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
.equ TUX64_BOOT_STAGE0_ADDR_PIF_RAM_HI,0xbfc0
.equ TUX64_BOOT_STAGE0_ADDR_PIF_RAM_LO,0x07c0

.equ TUX64_BOOT_STAGE0_ADDR_PIF_COMMAND_HI,TUX64_BOOT_STAGE0_ADDR_PIF_RAM_HI
.equ TUX64_BOOT_STAGE0_ADDR_PIF_COMMAND_LO,TUX64_BOOT_STAGE0_ADDR_PIF_RAM_LO+0x3c

.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE,              0x1000
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_MAGIC,             TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x00
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_CHECKSUM,          TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x04
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_LENGTH,            TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x0c
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_FLAGS,             TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x10
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_STAGE1_CHECKSUM,   TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x14
.equ TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_STAGE1_WORDS,      TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x18

.equ TUX64_BOOT_STAGE0_PIF_COMMAND_BOOT_TERMINATE,0x0008

.equ TUX64_BOOT_STAGE0_HEADER_MAGIC_HI,0x5442 /* TB */
.equ TUX64_BOOT_STAGE0_HEADER_MAGIC_LO,0x484d /* HM */

.equ TUX64_BOOT_STAGE0_FLAG_NO_CHECKSUM,0x0001

.equ TUX64_BOOT_STAGE0_PAYLOAD_MAX_WORDS_STAGE1,1024

.equ TUX64_BOOT_STAGE0_STATUS_BYTES,8

.equ TUX64_BOOT_STAGE0_STATUS_HWORD0,0x5354 /* ST */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD1,0x4147 /* AG */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD2,0x4530 /* E0 */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD3,0x3a00 /* :(null) */

.equ TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN,                         '0'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER_MAGIC,       '1'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER_CHECKSUM,    '2'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1_CODE_DATA,         '3'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1_CHECKSUM,         '4'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_SEND_BOOT_TERMINATE_COMMAND,   '5'
   
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
   or    $t0,$t0,$a0
   sw    $t0,tux64_boot_stage0_status+0x04($zero)
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

   # load and verify the boot header magic
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER_MAGIC
   jal   tux64_boot_stage0_status_code_write
   lui   $t0,TUX64_BOOT_STAGE0_HEADER_MAGIC_HI
   ori   $t0,$t0,TUX64_BOOT_STAGE0_HEADER_MAGIC_LO
   lw    $t1,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_MAGIC($s0)
   bne   $t0,$t1,tux64_boot_stage0_halt

   # load the boot header length and checksum and verify the length is at least
   # long enough to contain the magic and checksum
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER_CHECKSUM
   jal   tux64_boot_stage0_status_code_write
   lw    $s1,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_LENGTH($s0)
   sltiu $t0,$s1,3
   lw    $s2,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_CHECKSUM($s0)
   bnez  $t0,tux64_boot_stage0_halt

   # calculate the boot header checksum
   addiu $t0,$s0,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE+0x08 /* skip over magic + checksum */
   addiu $t1,$s1,-2 /* skip over magic + checksum */
   li    $t2,0 /* sum_hi */
   li    $t3,0 /* sum_lo */
   tux64_boot_stage0_start.header_checksum_calculate:
      lw    $t4,0($t0)
      addu  $t2,$t2,$t4
      addu  $t3,$t3,$t2
      addiu $t1,$t1,-1
      addiu $t0,$t0,4
      bnez  $t1,tux64_boot_stage0_start.header_checksum_calculate
   #tux64_boot_stage0_start.header_checksum_calculate
   subu  $t4,$t3,$t2

   # load the boot flags and verify the boot header checksum matches, done like
   # this to avoid assembler creating 'nop' instruction
   lw    $s3,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_FLAGS($s0)
   bne   $s2,$t4,tux64_boot_stage0_halt

   # load the boot flags, stage-1 checksum and length
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1_CODE_DATA
   jal   tux64_boot_stage0_status_code_write
   lw    $s4,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_STAGE1_CHECKSUM($s0)
   lw    $s5,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_STAGE1_WORDS($s0)

   # calculate pointers to the start of stage-1 data and RSP IMEM
   sll   $s1,$s1,2   /* convert word count to byte count */
   addu  $s1,$s1,$s0
   addiu $s1,$s1,TUX64_BOOT_STAGE0_ADDR_CART_ROM_OFFSET_BOOT_HEADER_BASE
   lui   $s0,TUX64_BOOT_STAGE0_ADDR_RSP_IMEM_HI
   ori   $s0,TUX64_BOOT_STAGE0_ADDR_RSP_IMEM_LO

   # begin loading the stage-1 data into RSP IMEM, as well as calculate its checksum
   addiu $t0,$s1,0   /* stage-1 data ptr */
   addiu $t1,$s0,0   /* RSP IMEM ptr */
   li    $t2,0       /* sum_hi */
   li    $t3,0       /* sum_lo */
   tux64_boot_stage0_start.stage1_load_checksum_calculate:
      lw    $t4,0($t0)
      addu  $t2,$t2,$t4
      addu  $t3,$t3,$t2
      sw    $t4,0($t1)
      addiu $s5,$s5,-1
      addiu $t0,$t0,4
      addiu $t1,$t1,4
      bnez  $s5,tux64_boot_stage0_start.stage1_load_checksum_calculate
   #tux64_boot_stage0_start.stage1_load_checksum_calculate
   subu  $s6,$t3,$t2

   # verify the stage-1 checksum matches if NO_CHECKSUM is set, also load the
   # pointers for PIF RAM to eliminate nop instructions being generated in the
   # branch delay slots and improve pipelining.
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1_CHECKSUM
   jal   tux64_boot_stage0_status_code_write
   andi  $t0,$s3,TUX64_BOOT_STAGE0_FLAG_NO_CHECKSUM
   lui   $s1,TUX64_BOOT_STAGE0_ADDR_PIF_COMMAND_HI
   li    $s2,TUX64_BOOT_STAGE0_PIF_COMMAND_BOOT_TERMINATE
   bnez  $t0,tux64_boot_stage0.skip_stage1_checksum
   bne   $s6,$s4,tux64_boot_stage0_halt
   tux64_boot_stage0.skip_stage1_checksum:

   # send the boot-termination command to the PIF, done last to give as much
   # time as possible for any previous SI bus write operations from IPL2 to
   # finish.  This is because the SI bus does writes asynchronously, so we must
   # wait for it to finish before issuing another write.  We can do this either
   # by checking the SI_STATUS MMIO register for IO_BUSY or DMA_BUSY, or we can
   # use a bogus read which will block the CPU until writing is complete.  The
   # bogus read is what we do below.
   li    $a0,TUX64_BOOT_STAGE0_STATUS_CODE_SEND_BOOT_TERMINATE_COMMAND
   jal   tux64_boot_stage0_status_code_write
   lw    $zero,TUX64_BOOT_STAGE0_ADDR_PIF_COMMAND_LO($s1)
   sw    $s2,TUX64_BOOT_STAGE0_ADDR_PIF_COMMAND_LO($s1)

   # execute the stage-1 bootloader, making room for the stage-1 code after the
   # stack pointer
   addiu $sp,$s0,-TUX64_BOOT_STAGE0_STATUS_BYTES
   jr    $s0
#tux64_boot_stage0_start

   .section .cic
tux64_boot_stage0_cic:
   # brute-forced using "ipl3hasher-new" by Polprzewodnikowy and rasky, as well
   # as my mighty AMD RX 6800 connected to my laptop via Thunderbolt 3 ;)
   .word 0x00002469
   .word 0xa8539ecc
#tux64_boot_stage0_cic

