#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/src/tux64-boot/stage0/stage0.s - Stage-0 bootloader code and CIC data.  #
#------------------------------------------------------------------------------#

# The stage-0 bootloader runs from RSP DMEM + 0x40, directly after the ROM
# header.  At this point, RDRAM is uninitialized, and the CPU's cache is
# uninitialized.  Our goal is to do the following:
# 
#  * Initialize COP0 registers to a consistent state
#  * Initialize RDRAM
#  * Initialze the CPU caches
#  * Load the boot header into RDRAM via PI DMA
#  * Verify the boot header's checksum
#  * Load the stage-1 binary into RDRAM via PI DMA
#  * Verify the stage-1 binary's checksum if boot flag NO_CHECKSUM isn't set
#  * Send the boot termination command to the PIF
#  * Move the stack into RDRAM
#  * Jump to the stage-1 start address
# 
# Now, the reason we're in assembly: DRM bullshit, yay!  More specifically,
# IPL2 verifies the checksum of the "IPL3" with the PIF, which will in turn
# halt the CPU if the checksum computed doesn't match the cartridge CIC's
# matching checksum.  This was done by Nintendo as a method to troll people
# attempting to write their own code in replacement of the IPL3.  However,
# there is a workaround.  We can use ipl3hasher(-new) to brute-force 8 bytes
# to place at the end of the stage-0 binary to force the checksum of our code to
# match the stock IPL3's checksum.  The upside is it does work, and we can write
# fully custom code with 0% of Nintendo's shit...but the downside is we need to
# recompute the bytes every time we change the code.  To prevent pissing off
# non-developers, we write the entire stage-0 in assembly so we can get 100%
# consistent machine code, then simply distribute the brute-forced bytes with
# the source code so only developers modifying stage-0 have to compute the
# checksum bytes.  For the rest of the boot process, we have full control over
# the system, so we can use C instead of assembly.
# 
# Note #1: due to the way the SysAD bus and PI+SI+RCP handles reads and writes
# to their respective MMIO address spaces, we need to be careful when accessing
# memory.  For RSP IMEM/DMEM, any size read is allowed, but only 32-bit writes
# are valid.  For PI+SI-mapped data, only 32-bit reads are allowed, and only
# 32-bit writes are allowed when the I/O busy/DMA busy bits aren't set in the
# PI_STATUS/SI_STATUS registers.  Accesses of all sizes are allowed in RDRAM
# once its initialized.
# 
# Note #2: we disable assembler reordering of instructions, use of $at, and
# assembler macros. this is done so we have 100% consistent machine code,
# ideally no matter what assembler or assembler flags are in use.  do remember,
# though, that branch delay slots lead to 1 instruction after most branch
# instructions to be executed.  This does make it even more difficult to
# program, but just remember...no pain, no gain!

.set noreorder
.set noat
.set nomacro

.equ TUX64_BOOT_STAGE0_ADDRESS_RSP_DMEM_HI,0xa400
.equ TUX64_BOOT_STAGE0_ADDRESS_RSP_DMEM_LO,0x0000

.equ TUX64_BOOT_STAGE0_HEADER_MAGIC_HI,0x5442 /* TB */
.equ TUX64_BOOT_STAGE0_HEADER_MAGIC_LO,0x484d /* HM */

.equ TUX64_BOOT_STAGE0_FLAG_NO_CHECKSUM,0x0001

.equ TUX64_BOOT_STAGE0_PAYLOAD_MAX_WORDS_STAGE1,1024

.equ TUX64_BOOT_STAGE0_STATUS_BYTES,8

.equ TUX64_BOOT_STAGE0_STATUS_ADDRESS_HI,TUX64_BOOT_STAGE0_ADDRESS_RSP_DMEM_HI
.equ TUX64_BOOT_STAGE0_STATUS_ADDRESS_LO,TUX64_BOOT_STAGE0_ADDRESS_RSP_DMEM_LO+0x0ff8

.equ TUX64_BOOT_STAGE0_STATUS_HWORD0,0x5354 /* ST */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD1,0x4147 /* AG */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD2,0x4530 /* E0 */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD3,0x3a00 /* :(null) */

.equ TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN,                '0'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_COP0_INITIALIZE,      '1'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_RDRAM_INITIALIZE,     '2'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CPU_CACHE_INITIALIZE, '3'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_BOOT_HEADER,     '4'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER,    '5'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1,          '6'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1,         '7'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_PIF_TERMINATE_BOOT,   '8'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_START_STAGE1,         '9'
   
   .section .status
tux64_boot_stage0_status:
   # This is a chunk of memory placed at the end of RSP DMEM to show the current
   # boot status in memory to help debug early boot issues with memory
   # debuggers.  It will look like "STAGE0:c" when memory is displayed as ASCII
   # text, where 'c' is the current boot stage code.
   .skip TUX64_BOOT_STAGE0_STATUS_BYTES
#tux64_boot_stage0_status

   .section .text
tux64_boot_stage0_status_code_write:
   lui   $t0,TUX64_BOOT_STAGE0_STATUS_HWORD2
   ori   $t0,$t0,TUX64_BOOT_STAGE0_STATUS_HWORD3
   lui   $t1,TUX64_BOOT_STAGE0_STATUS_ADDRESS_HI
   or    $t0,$t0,$a0
   jr    $ra
   sw    $t0,TUX64_BOOT_STAGE0_STATUS_ADDRESS_LO+0x04($t1)
#tux64_boot_stage0_status_code_write

   .section .text
tux64_boot_stage0_halt:
   b     tux64_boot_stage0_halt
#tux64_boot_stage0_halt

   .section .start
tux64_boot_stage0_start:
   # initialize status identifier and code
   lui   $t0,TUX64_BOOT_STAGE0_STATUS_HWORD0
   ori   $t0,$t0,TUX64_BOOT_STAGE0_STATUS_HWORD1
   lui   $t1,TUX64_BOOT_STAGE0_STATUS_ADDRESS_HI
   sw    $t0,TUX64_BOOT_STAGE0_STATUS_ADDRESS_LO($t1)
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN

   # TODO: implement
   b     tux64_boot_stage0_halt
#tux64_boot_stage0_start

   .section .cic
tux64_boot_stage0_cic:
   # brute-forced using "ipl3hasher-new" by Polprzewodnikowy and rasky, as well
   # as my mighty AMD RX 6800 connected to my laptop via Thunderbolt 3 ;)
   .word 0x00000000
   .word 0x00000000
#tux64_boot_stage0_cic

