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

.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE,           0x1000
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_MAGIC_HI32,     TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x00
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_MAGIC_LO32,     TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x04
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_WORDS,          TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x0e
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_STAGE1_LENGTH,  TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x1e

.equ TUX64_BOOT_STAGE0_PAYLOAD_MAX_WORDS_STAGE1,1024

.equ TUX64_BOOT_STAGE0_STATUS_HWORD0, 0x5354 /* ST */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD1, 0x4147 /* AG */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD2, 0x4530 /* E0 */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD3, 0x3a00 /* :(null) */

.equ TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN,                   '0'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER_MAGIC, '1'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_BOOT_HEADER_LENGTH, '2'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1_LENGTH,      '3'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1_LENGTH,     '4'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CALCULATE_POINTERS,      '5'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1,             '6'
   
   .section .status
tux64_boot_stage0_status:
   # This is a chunk of memory placed at the start of RSP IMEM to show the
   # current boot status in memory to help debug early boot issues with
   # memory debuggers.  It will look like "STAGE0:c" when memory is displayed
   # as ASCII text, where 'c' is the current boot stage code.  Note that once
   # stage-1 is loaded, this will be overwritten and a seperate memory address
   # defined in stage-1 will contain another early-boot status code.
   .skip 8
#tux64_boot_stage0_status

   .section .rodata
tux64_boot_stage0_boot_header_magic:
   .ascii "TUX64LDR"
#tux64_boot_stage0_boot_header_magic

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

   # read and verify the boot header magic value
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER_MAGIC
   jal   tux64_boot_stage0_status_code_write
   lw    $t0,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_MAGIC_HI32($s0)
   lw    $t1,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_MAGIC_LO32($s0)
   lw    $t2,tux64_boot_stage0_boot_header_magic+0x00($zero)
   lw    $t3,tux64_boot_stage0_boot_header_magic+0x04($zero)
   bne   $t0,$t2,tux64_boot_stage0_halt
   bne   $t1,$t3,tux64_boot_stage0_halt

   # read the header length and stage1 length, has do be done cleverly due to
   # the busted SysAD implementation for the PI external bus
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_BOOT_HEADER_LENGTH
   jal   tux64_boot_stage0_status_code_write
   lw    $s1,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_WORDS-0x02($s0)
   andi  $s1,$s1,0xffff
   li    $a1,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1_LENGTH
   jal   tux64_boot_stage0_status_code_write
   lw    $s2,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_STAGE1_LENGTH-0x02($s0)
   andi  $s2,$s2,0xffff

   # verify the length of the stage-1 binary so our memcpy won't fuck out
   # (0 < stage1 words <= 1024), this also protects the boot process from a
   # corrupted ROM image beyond just checking the magic
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1_LENGTH
   jal   tux64_boot_stage0_status_code_write
   beq   $s2,$zero,tux64_boot_stage0_halt
   sltiu $t0,$s2,TUX64_BOOT_STAGE0_PAYLOAD_MAX_WORDS_STAGE1+1
   beq   $t0,$zero,tux64_boot_stage0_halt

   # calculate the pointers to set up the memcpy to load the stage-1 binary into
   # RSP IMEM.
   # for reference, here are the current registers we care about:
   #    $s0 - ROM MMIO base address (0xb0000000)
   #    $s1 - boot header length, in 32-bit words
   #    $s2 - stage-1 binary length, in 32-bit words
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_CALCULATE_POINTERS
   jal   tux64_boot_stage0_status_code_write
   sll   $t0,$s1,2
   addiu $s0,$s0,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE
   addu  $s0,$s0,$t0
   lui   $s1,TUX64_BOOT_STAGE0_ADDR_RSP_IMEM_HI
   ori   $s1,TUX64_BOOT_STAGE0_ADDR_RSP_IMEM_LO

   # our registers now contain the following:
   #    $s0 = memcpy source address
   #    $s1 = memcpy dest address, aka top of RSP IMEM
   #    $s2 = stage-1 binary words to copy
   # 
   # now we load the stage-1 binary into RSP IMEM, overwriting any current .bss
   # data.  we also copy the RSP IMEM address for use when setting up the stack
   # and executing stage-1.
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1
   jal   tux64_boot_stage0_status_code_write
   addiu $s3,$s1,0
   tux64_boot_stage0_start.load_stage1:
      lw    $t0,0($s0)
      sw    $t0,0($s1)
      addiu $s0,$s0,4
      addiu $s1,$s1,4
      addiu $s2,$s2,-1
      bne   $s2,$zero,tux64_boot_stage0_start.load_stage1
   #tux64_boot_stage0_start.load_stage1

   # move the stack to the top of RSP DMEM and execute the stage-1 binary
   addiu $sp,$s3,0
   jr    $s3
#tux64_boot_stage0_start

   .section .cic
tux64_boot_stage0_cic:
   # brute-forced using "ipl3hasher-new" by Polprzewodnikowy and rasky, as well
   # as my mighty AMD RX 6800 connected to my laptop via Thunderbolt 3 ;)
   .word 0x00006ed0
   .word 0x64f1f9d9
#tux64_boot_stage0_cic

