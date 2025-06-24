#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/src/tux64-boot/stage0/stage0.s - Stage-0 bootloader code and CIC data.  #
#------------------------------------------------------------------------------#

# The stage-0 bootloader runs from RSP DMEM + 0x40, directly after the ROM
# header.  At this point, RDRAM is uninitialized, and the CPU's cache is
# uninitialized.  The main goal of the stage-0 bootloader is to load the
# stage-1 bootloader into RSP IMEM.  We leave initialization of RDRAM and the
# CPU's cache to stage-1 because stage-0 must match the CIC-6102 checksum.
# This is some copy-protection crap where the IPL2 uses the cartridge's CIC
# chip to compute a checksum of the expected IPL3.  If they don't match, the
# CPU will have NMI permanently held down until the console reboots.
# 
# We use assembly here because we must have 100% replicable machine code, as
# to bypass the IPL2's and CIC's protection, we brute-force the checksum to
# match, and thus require consistent machine code.  The rest of the boot
# process uses C instead.
# 
# Note: due to implementation details of the SysAD bus, we need to be careful
# with read/write sizes.  To put it simply, reads from RCP memory are fine
# except for 64-bit reads, non 32-bit writes to RCP memory are treated as
# 32-bit writes and spill register contents, and non 32-bit reads and writes
# to PI-bus controlled data is completely disallowed.

.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM,0xb000
.equ TUX64_BOOT_STAGE0_ADDR_RSP_IMEM_HI,0xa400
.equ TUX64_BOOT_STAGE0_ADDR_RSP_IMEM_LO,0x1000

.equ TUX64_BOOT_STAGE0_HEADER_MAGIC_HI,0x5442 /* TB */
.equ TUX64_BOOT_STAGE0_HEADER_MAGIC_LO,0x4c4d /* LM */

.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE,              0x1000
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_MAGIC,             TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x00
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_CHECKSUM,          TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x04
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_LENGTH,            TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x0c
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_STAGE1_CHECKSUM,   TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x10
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_STAGE1_WORDS,      TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x14

.equ TUX64_BOOT_STAGE0_PAYLOAD_MAX_WORDS_STAGE1,1024

.equ TUX64_BOOT_STAGE0_STATUS_HWORD0, 0x5354 /* ST */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD1, 0x4147 /* AG */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD2, 0x4530 /* E0 */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD3, 0x3a00 /* :(null) */

.equ TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN,                      '0'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER_MAGIC,    '1'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER_CHECKSUM, '2'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1_CODE_DATA,      '3'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1_CHECKSUM,      '4'
   
   .section .status
tux64_boot_stage0_status:
   # This is a chunk of memory placed at the end of RSP DMEM to show the current
   # boot status in memory to help debug early boot issues with memory
   # debuggers.  It will look like "STAGE0:c" when memory is displayed as ASCII
   # text, where 'c' is the current boot stage code.
   .skip 8
#tux64_boot_stage0_status

   .section .text
tux64_boot_stage0_status_code_write:
   lui   $t0,TUX64_BOOT_STAGE0_STATUS_HWORD2
   ori   $t0,$t0,TUX64_BOOT_STAGE0_STATUS_HWORD3
   or    $t0,$t0,$a0
   sw    $t0,tux64_boot_stage0_status+0x04
   jr    $ra
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
   sw    $t0,tux64_boot_stage0_status+0x00($zero)
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN
   jal   tux64_boot_stage0_status_code_write

   # reserve $s0 for the base offset into the cartridge ROM
   lui   $s0,TUX64_BOOT_STAGE0_ADDR_CART_ROM

   # load and verify the bootloader magic
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER_MAGIC
   jal   tux64_boot_stage0_status_code_write
   lui   $t0,TUX64_BOOT_STAGE0_HEADER_MAGIC_HI
   ori   $t0,$t0,TUX64_BOOT_STAGE0_HEADER_MAGIC_LO
   lw    $t1,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_MAGIC($s0)
   bne   $t0,$t1,tux64_boot_stage0_halt

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

