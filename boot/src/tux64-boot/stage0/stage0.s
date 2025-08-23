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
# memory.  For RSP IMEM/DMEM, all read sizes except 64-bit are allowed, but only
# 32-bit writes are valid.  For PI+SI-mapped data, only 32-bit reads are allowed,
# allowed, and only 32-bit writes are allowed when the I/O busy/DMA busy bits
# aren't set in the PI_STATUS/SI_STATUS registers.  Accesses of all sizes are
# allowed in RDRAM once its initialized.
# 
# Note #2: we disable assembler reordering of instructions, use of $at, and
# assembler macros. this is done so we have 100% consistent machine code,
# ideally no matter what assembler or assembler flags are in use.  do remember,
# though, that branch delay slots lead to 1 instruction after most branch
# instructions to be executed.  This does make it even more difficult to
# program, but just remember...no pain, no gain!
# 
# Note #3: For optimization, we use a custom calling convention that goes like
# this:
# 
# $t0-$t3:           arguments, volatile
# $t4-$t7, $at:      volatile
# $v0-$v1:           return addresses, volatile
# everything else:   non-volatile

.set noreorder
.set noat
.set nomacro

.equ TUX64_BOOT_STAGE0_COP0_REGISTER_COUNT,$9
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_COMPARE,$11
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_STATUS,$12
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_CAUSE,$13
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_TAGLO,$28
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_TAGHI,$29

.equ TUX64_BOOT_STAGE0_COP0_STATUS_FLAGS_LOW_POWER,((1 << 27) >> 16)

.equ TUX64_BOOT_STAGE0_ICACHE_BYTES_PER_LINE,32
.equ TUX64_BOOT_STAGE0_DCACHE_BYTES_PER_LINE,16
.equ TUX64_BOOT_STAGE0_ICACHE_LINE_COUNT,512
.equ TUX64_BOOT_STAGE0_DCACHE_LINE_COUNT,512

.equ TUX64_BOOT_STAGE0_CACHE_TYPE_ICACHE,0x0000
.equ TUX64_BOOT_STAGE0_CACHE_TYPE_DCACHE,0x0001
.equ TUX64_BOOT_STAGE0_CACHE_INDEX_STORE_TAG,0x0008

.equ TUX64_BOOT_STAGE0_ADDRESS_RDRAM_CACHED_HI,0x8000
.equ TUX64_BOOT_STAGE0_ADDRESS_RDRAM_UNCACHED_HI,0xa000

.equ TUX64_BOOT_STAGE0_ADDRESS_RSP_DMEM_HI,0xa400
.equ TUX64_BOOT_STAGE0_ADDRESS_RSP_DMEM_LO,0x0000

.equ TUX64_BOOT_STAGE0_ADDRESS_MI_HI,0xa430
.equ TUX64_BOOT_STAGE0_ADDRESS_MI_LO,0x0000

.equ TUX64_BOOT_STAGE0_ADDRESS_MI_MODE_LO,TUX64_BOOT_STAGE0_ADDRESS_MI_LO+0x0000
.equ TUX64_BOOT_STAGE0_ADDRESS_MI_VERSION_LO,TUX64_BOOT_STAGE0_ADDRESS_MI_LO+0x0004

.equ TUX64_BOOT_STAGE0_MI_MODE_REPEAT_16,0x010f

.equ TUX64_BOOT_STAGE0_ADDRESS_PI_HI,0xa460
.equ TUX64_BOOT_STAGE0_ADDRESS_PI_LO,0x0000

.equ TUX64_BOOT_STAGE0_PI_DMA_CARTRIDGE_ROM_ADDRESS_HI,0x1000

.equ TUX64_BOOT_STAGE0_ADDRESS_PI_DRAM_ADDR_LO,0x0000
.equ TUX64_BOOT_STAGE0_ADDRESS_PI_CART_ADDR_LO,0x0004
.equ TUX64_BOOT_STAGE0_ADDRESS_PI_WR_LEN_LO,0x000c
.equ TUX64_BOOT_STAGE0_ADDRESS_PI_STATUS,0x0010

.equ TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY,0x0003

.equ TUX64_BOOT_STAGE0_ADDRESS_RI_HI,0xa470
.equ TUX64_BOOT_STAGE0_ADDRESS_RI_LO,0x0000

.equ TUX64_BOOT_STAGE0_ADDRESS_RI_MODE_LO,TUX64_BOOT_STAGE0_ADDRESS_RI_LO+0x0000
.equ TUX64_BOOT_STAGE0_ADDRESS_RI_CONFIG_LO,TUX64_BOOT_STAGE0_ADDRESS_RI_LO+0x0004
.equ TUX64_BOOT_STAGE0_ADDRESS_RI_CURRENT_LOAD_LO,TUX64_BOOT_STAGE0_ADDRESS_RI_LO+0x0008
.equ TUX64_BOOT_STAGE0_ADDRESS_RI_SELECT_LO,TUX64_BOOT_STAGE0_ADDRESS_RI_LO+0x000c

.equ TUX64_BOOT_STAGE0_RI_CONFIG_AUTO_CURRENT_CALIBRATION,0x0040
.equ TUX64_BOOT_STAGE0_RI_RX_TX_TIMINGS,0x0014
.equ TUX64_BOOT_STAGE0_RI_MODE_STANDARD,0x000e

.equ TUX64_BOOT_STAGE0_ADDRESS_CARTRIDGE_ROM_HI,0xb000
.equ TUX64_BOOT_STAGE0_ADDRESS_CARTRIDGE_ROM_LO,0x0000

.equ TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_HI,0xbfc0
.equ TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_LO,0x07c0

.equ TUX64_BOOT_STAGE0_MI_VERSION_IQUE_HI,0x0202
.equ TUX64_BOOT_STAGE0_MI_VERSION_IQUE_LO,0xb0b0

.equ TUX64_BOOT_STAGE0_ADDRESS_RDRAM_REGISTERS_BROADCAST_HI,0xa3f8
.equ TUX64_BOOT_STAGE0_ADDRESS_RDRAM_REGISTERS_BROADCAST_LO,0x0000

.equ TUX64_BOOT_STAGE0_RDRAM_REGISTER_DELAY_LO,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_REGISTERS_BROADCAST_LO+0x0008

.equ TUX64_BOOT_STAGE0_RDRAM_DELAY_TIMING,0x18082838

.equ TUX64_BOOT_STAGE0_PIF_COMMAND_TERMINATE_BOOT,0x0008

.equ TUX64_BOOT_STAGE0_1MIB_HI,0x0010

.equ TUX64_BOOT_STAGE0_STAGE_1_STACK_SIZE,0x1000

.equ TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES,0x0038
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES_ALIGN16,TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES+0x08
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_LO,0x1000
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_RDRAM_LO,TUX64_BOOT_STAGE0_STAGE_1_STACK_SIZE

.equ TUX64_BOOT_STAGE0_BOOT_HEADER_WORD_SIZE_POW2_EXPONENT,1
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_WORD_SIZE,(2 << TUX64_BOOT_STAGE0_BOOT_HEADER_WORD_SIZE_POW2_EXPONENT)

.equ TUX64_BOOT_STAGE0_BOOT_HEADER_MAGIC_HI,0x5442 /* TB */
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_MAGIC_LO,0x484d /* HM */

.equ TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_MAGIC,0x0000
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_CHECKSUM,0x0004
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA,0x0008
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA_FLAGS,0x0008
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_CHECKSUM,0x000c
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_LENGTH_WORDS,0x0010
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_MEMORY_WORDS,0x0014

.equ TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_CARTRIDGE_ROM_LO,TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_LO+TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES
.equ TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_RDRAM_LO,TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_RDRAM_LO+TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES_ALIGN16

.equ TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_REQUIRED_ADDITIONAL_MEMORY,(TUX64_BOOT_STAGE0_STAGE_1_STACK_SIZE + TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES_ALIGN16)

.equ TUX64_BOOT_STAGE0_FLAG_NO_CHECKSUM,0x0001

.equ TUX64_BOOT_STAGE0_STATUS_BYTES,8

.equ TUX64_BOOT_STAGE0_STATUS_HWORD0,0x5354 /* ST */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD1,0x4147 /* AG */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD2,0x4530 /* E0 */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD3,0x3a00 /* :(null) */

# we store the upper and lower byte of the status code together to save on an
# 'or' instruction for free in tux64_boot_stage0_status_code_write().
.equ TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN,                         (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'A')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_COP0_INITIALIZE,               (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'B')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_HARDWARE_INFORMATION,   (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'C')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_RDRAM_INITIALIZE,              (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'D')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_TOTAL_MEMORY,           (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'E')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_BOOT_HEADER,              (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'F')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CPU_CACHE_INITIALIZE,          (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'G')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER,             (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'H')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1,                   (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'I')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1,                  (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'J')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_PIF_TERMINATE_BOOT,            (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'K')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_START_STAGE1,                  (TUX64_BOOT_STAGE0_STATUS_HWORD3 | 'L')

# defined in stage1/boot-header.ld, included in stage0/stage0.ld
.extern tux64_boot_stage1_boot_header

   .section .status
tux64_boot_stage0_status:
   # This is a chunk of memory placed at the end of RSP DMEM to show the current
   # boot status in memory to help debug early boot issues with memory
   # debuggers.  It will look like "STAGE0:c" when memory is displayed as ASCII
   # text, where 'c' is the current boot stage code.
   .skip TUX64_BOOT_STAGE0_STATUS_BYTES
#tux64_boot_stage0_status

   .section .text
tux64_boot_stage0_halt:
   # set the processor into low-power mode so we don't waste power before
   # entering the infinite spinlock.  we don't care about updating RDRAM refresh
   # rate or anything since we don't do anything else from this point.  memory
   # will be re-initialized on next startup, so memory decaying away isn't our
   # problem.

   # bogus read to to uncached memory to force SysAD to be inactive before
   # enabling low-power mode, per the processor's documentation
   lw    $zero,0($gp)

   # set the RP bit in the COP0 Status register
   lui   $t1,TUX64_BOOT_STAGE0_COP0_STATUS_FLAGS_LOW_POWER
   mfc0  $t0,TUX64_BOOT_STAGE0_COP0_REGISTER_STATUS
   or    $t0,$t0,$t1
   mtc0  $t0,TUX64_BOOT_STAGE0_COP0_REGISTER_STATUS

   # we are now met with a terrible fate.
   tux64_boot_stage0_halt.spinlock:
   b     tux64_boot_stage0_halt.spinlock
   nop
#tux64_boot_stage0_halt

   .section .text
tux64_boot_stage0_status_code_write:
   # $t0 contains HWORD3 or'd with the status code already, so we only need to
   # load the upper 2 bytes.
   lui   $t1,TUX64_BOOT_STAGE0_STATUS_HWORD2
   or    $t1,$t1,$t0
   jr    $ra
   sw    $t1,%lo(tux64_boot_stage0_status)+0x04($gp)
#tux64_boot_stage0_status_code_write

   .section .text
tux64_boot_stage0_checksum_calculate_and_verify:
   # calculates a checksum and verifies it's correct, halting if it's not
   # correct and returning if it is correct.  data must be word-aligned.
   # 
   # $t0 - data start ptr
   # $t1 - data end ptr
   # $t2 - expected checksum

   addiu $t3,$zero,0 # sum_hi
   addiu $t4,$zero,0 # sum_lo
   tux64_boot_stage0_checksum_calculate_and_verify.digest_word:
      lw    $t5,0($t0)
      addu  $t3,$t3,$t5
      addu  $t4,$t4,$t3
      addiu $t0,$t0,4
      bne   $t0,$t1,tux64_boot_stage0_checksum_calculate_and_verify.digest_word
   #tux64_boot_stage0_checksum_calculate_and_verify.digest_word

   subu  $t6,$t4,$t3 # executes in branch delay slot of loop above
   bne   $t6,$t2,tux64_boot_stage0_halt

   nop
   jr    $ra
   nop
#tux64_boot_stage0_checksum_calculate_and_verify

   .section .text
tux64_boot_stage0_rdram_wait:
   addiu $t0,$zero,0x0100
   tux64_boot_stage0_rdram_wait.loop:
      addiu $t0,$t0,-1
      bne   $t0,$zero,tux64_boot_stage0_rdram_wait.loop
      nop
   #tux64_boot_stage0_rdram_wait.loop

   jr    $ra
   nop
#tux64_boot_stage0_rdram_wait

   .section .start
   .global tux64_boot_stage0_start
tux64_boot_stage0_start:
   # for starters, we have the following reserved registers from IPL2 which will
   # later be passed to stage-1:
   # $s3 - ROM type (stock/64DD)
   # $s4 - video standard (NTSC, PAL, M-PAL)
   # $s5 - reset type
   # $s6 - ROM CIC seed
   # $s7 - PIF ROM version

   # reserve $gp for the start of RSP DMEM, used everywhere to cut down on code
   lui   $gp,TUX64_BOOT_STAGE0_ADDRESS_RSP_DMEM_HI

   # initialize status identifier and code
   lui   $t2,TUX64_BOOT_STAGE0_STATUS_HWORD0
   ori   $t2,$t2,TUX64_BOOT_STAGE0_STATUS_HWORD1
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN
   jal   tux64_boot_stage0_status_code_write
   sw    $t2,%lo(tux64_boot_stage0_status)+0x00($gp)

   # begin initializing COP0 registers
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_COP0_INITIALIZE

   # i'm not entirely sure why everyone initializes these specific registers.
   # it seems it's to clear exceptions and some timer thing from a reset?
   # nobody anywhere explains why, so it looks like i'll be copying the person
   # who copied the person who copied the person who wrote the official IPL3.
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_CAUSE
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_COUNT
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_COMPARE

   # begin detecting hardware information
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_HARDWARE_INFORMATION

   # reserve $fp for the MI base address until the end of RDRAM initialization
   lui   $fp,TUX64_BOOT_STAGE0_ADDRESS_MI_HI

   # detect the hardware revision, which is used in RDRAM initialization and to
   # detect ique support.  we then reserve $s0 for the hardware revision until
   # we detect HW1/HW2 RDRAM and $a2 for the boolean if we're on ique or not.
   # we then skip RDRAM initialization if we're on ique, as there's no RDRAM to
   # initialize.
   lui   $t0,TUX64_BOOT_STAGE0_MI_VERSION_IQUE_HI
   lw    $t1,TUX64_BOOT_STAGE0_ADDRESS_MI_VERSION_LO($fp)
   ori   $t0,$t0,TUX64_BOOT_STAGE0_MI_VERSION_IQUE_LO
   xor   $a2,$t0,$t1
   andi  $s0,$t2,0x00ff
   beq   $t0,$t1,tux64_boot_stage0_start.skip_rdram_initialization
   sltiu $a2,$a2,1 # branch delay slot

   # begin initializing RDRAM
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_RDRAM_INITIALIZE

   # reserve $s1 for the RI base address until the end of RDRAM initialization
   lui   $s1,TUX64_BOOT_STAGE0_ADDRESS_RI_HI

   # check if RDRAM has already been initialized
   lw    $t0,TUX64_BOOT_STAGE0_ADDRESS_RI_SELECT_LO($s1)
   andi  $t0,$t0,0x00ff # mask out undefined bits
   bne   $t0,$zero,tux64_boot_stage0_start.skip_rdram_initialization

   # i'll admit, the RDRAM code below is mostly referenced from libdragon, as
   # this shit is too complex to free-style.

   # perform current calibration
   addiu $t1,$zero,TUX64_BOOT_STAGE0_RI_CONFIG_AUTO_CURRENT_CALIBRATION # branch delay slot
   jal   tux64_boot_stage0_rdram_wait
   sw    $t1,TUX64_BOOT_STAGE0_ADDRESS_RI_CONFIG_LO($s1) # branch delay slot
   sw    $zero,TUX64_BOOT_STAGE0_ADDRESS_RI_CURRENT_LOAD_LO($s1)

   # configure rx/tx signal timings
   addiu $t0,$zero,TUX64_BOOT_STAGE0_RI_RX_TX_TIMINGS
   sw    $t0,TUX64_BOOT_STAGE0_ADDRESS_RI_SELECT_LO($s1)

   # reset all the chips
   jal   tux64_boot_stage0_rdram_wait
   sw    $zero,TUX64_BOOT_STAGE0_ADDRESS_RI_MODE_LO($s1) # RI_MODE = reset, branch delay slot
   addiu $t0,$zero,TUX64_BOOT_STAGE0_RI_MODE_STANDARD
   jal   tux64_boot_stage0_rdram_wait
   sw    $t0,TUX64_BOOT_STAGE0_ADDRESS_RI_MODE_LO($s1) # branch delay slot

   # reserve $s2 for the spacing between RDRAM registers.  HW1 RDRAM, found on
   # early development boards, is spaced out 0x200 bytes.  Everything else is
   # spaced out 0x400 bytes.  also frees $s0.
   addiu $t0,$zero,1
   beq   $t0,$s0,tux64_boot_stage0_start.rdram_hw1
   addiu $s2,$zero,0x200 # branch delay slot
   addiu $s2,$s2,0x200
   tux64_boot_stage0_start.rdram_hw1:

   # initialize Delay register.  see libdragon/boot/rdram.c:142 for more
   # information.  also loads the RDRAM broadcast registers base address into
   # $t2.
   lui   $t0,%hi(TUX64_BOOT_STAGE0_RDRAM_DELAY_TIMING)
   addiu $t1,$zero,TUX64_BOOT_STAGE0_MI_MODE_REPEAT_16
   lui   $t2,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_REGISTERS_BROADCAST_HI
   ori   $t0,$t0,%lo(TUX64_BOOT_STAGE0_RDRAM_DELAY_TIMING)
   sw    $t1,TUX64_BOOT_STAGE0_ADDRESS_MI_MODE_LO($fp)
   sw    $t0,TUX64_BOOT_STAGE0_RDRAM_REGISTER_DELAY_LO($t2)

   # TODO initialize the device IDs for every chip


   # TODO: implement per-chip initialization loop


   # TODO: configure RI refresh rate
   b     tux64_boot_stage0_halt
   nop

   tux64_boot_stage0_start.skip_rdram_initialization:

   # detect the total amount of available memory.  this is done by probing
   # memory 1MiB at a time.  this number is chosen because the smallest size of
   # RDRAM chips is 1MiB (despite never being used in the N64), so that's the
   # smallest reasonable size to probe.  for the ique player, we always have
   # 8MiB of available memory (technically 16MiB, but the RI only exposes 8MiB),
   # so this code still works.  we cap the upper limit to 8MiB since that's the
   # maximum amount of memory possible.
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_TOTAL_MEMORY

   addiu $a0,$zero,0
   lui   $s0,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_UNCACHED_HI
   lui   $s1,TUX64_BOOT_STAGE0_1MIB_HI
   lui   $s2,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_UNCACHED_HI + (TUX64_BOOT_STAGE0_1MIB_HI * 8)
   tux64_boot_stage0_start.detect_total_memory:
      ld    $t3,0($s0)     # load current value
      nor   $t4,$t3,$zero  # flips all bits
      sd    $t4,0($s0)     # store the flipped bits
      ld    $t5,0($s0)     # reload to see what was stored
      sd    $t3,0($s0)     # restore the old value

      # if the values differ, then either the memory chip is bad or we're at the
      # end of available memory.  exit the loop.
      bne   $t4,$t5,tux64_boot_stage0_start.detect_total_memory.exit

      # advance the memory pointer using the branch delay slot
      addu  $s0,$s0,$s1

      # the maximum amount of possible memory is 8MiB.  if we're not past 8MiB
      # of memory, continue looping
      bne   $s0,$s2,tux64_boot_stage0_start.detect_total_memory

      # increment the total detected memory using the branch delay slot
      addu  $a0,$a0,$s1
   #tux64_boot_stage0_start.detect_total_memory
   tux64_boot_stage0_start.detect_total_memory.exit:

   # from this point forward, $a0 is reserved for the total system memory, in
   # bytes until stage-1 begins.  this will be passed as an argument to stage-1
   # to avoid using memory unnecessarily.

   # load the boot header into RDRAM via PI DMA
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_BOOT_HEADER

   # reserve $s0 for the PI base address and $s1 for the high-order cartridge
   # ROM offset
   lui   $s0,TUX64_BOOT_STAGE0_ADDRESS_PI_HI
   lui   $s1,TUX64_BOOT_STAGE0_PI_DMA_CARTRIDGE_ROM_ADDRESS_HI

   # halt any active PI DMA operations.  this needs to be done because we could
   # be coming from a reset in the middle if a PI DMA operation.
   addiu $t2,$zero,TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY
   sw    $t2,TUX64_BOOT_STAGE0_ADDRESS_PI_STATUS($s0)

   # initiate the transfer in the background, we don't need to invalidate cache
   # as we're initializing it below anyways.  also note we don't need to wait on
   # any previous PI DMA operations since we halted them above
   addiu $t3,$zero,%lo(tux64_boot_stage1_boot_header)
   addiu $t4,$zero,(TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES - 1)
   ori   $t5,$s1,TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_LO
   sw    $t3,TUX64_BOOT_STAGE0_ADDRESS_PI_DRAM_ADDR_LO($s0)
   sw    $t5,TUX64_BOOT_STAGE0_ADDRESS_PI_CART_ADDR_LO($s0)
   sw    $t4,TUX64_BOOT_STAGE0_ADDRESS_PI_WR_LEN_LO($s0)

   # initialize the CPU caches, setting each line to 'invalid', continue
   # loading the boot header in the background
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_CPU_CACHE_INITIALIZE
   lui   $t2,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_CACHED_HI
   lui   $t3,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_CACHED_HI
   lui   $s2,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_CACHED_HI
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_TAGLO # write 'invalid' tag
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_TAGHI # must be zero, undefined on boot
   addiu $t2,$t2,(TUX64_BOOT_STAGE0_ICACHE_BYTES_PER_LINE * TUX64_BOOT_STAGE0_ICACHE_LINE_COUNT)
   addiu $t3,$t3,(TUX64_BOOT_STAGE0_DCACHE_BYTES_PER_LINE * TUX64_BOOT_STAGE0_DCACHE_LINE_COUNT)
   tux64_boot_stage0_start.initialize_cache:
      addiu $t2,$t2,-TUX64_BOOT_STAGE0_ICACHE_BYTES_PER_LINE
      addiu $t3,$t3,-TUX64_BOOT_STAGE0_DCACHE_BYTES_PER_LINE
      cache (TUX64_BOOT_STAGE0_CACHE_TYPE_ICACHE | TUX64_BOOT_STAGE0_CACHE_INDEX_STORE_TAG),0($t2)
      bne   $s2,$t2,tux64_boot_stage0_start.initialize_cache
      cache (TUX64_BOOT_STAGE0_CACHE_TYPE_DCACHE | TUX64_BOOT_STAGE0_CACHE_INDEX_STORE_TAG),0($t3)
   #tux64_boot_stage0_start.initialize_cache

   # we will now reserve $s2 for the cached RDRAM base address. here are the
   # current reserved registers:
   # $a0       - total memory
   # $a2       - ique boolean
   # $s0       - PI MMIO registers base address
   # $s1       - cartridge ROM base address (physical address)
   # $s2       - cached RDRAM base address
   # $s3-$s7   - IPL2 arguments

   # calculate the checksum of the boot header and verify it matches what's
   # given by the header
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER

   # wait for the boot header to have finished loading
   tux64_boot_stage0_start.pi_io_dma_spinlock.boot_header:
      lw    $at,TUX64_BOOT_STAGE0_ADDRESS_PI_STATUS($s0)
      andi  $at,$at,TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY
      bne   $at,$zero,tux64_boot_stage0_start.pi_io_dma_spinlock.boot_header
   #tux64_boot_stage0_start.pi_io_dma_spinlock.boot_header

   # verify the boot header magic is present
   lui   $t0,TUX64_BOOT_STAGE0_BOOT_HEADER_MAGIC_HI # branch delay slot
   lw    $t1,%lo(tux64_boot_stage1_boot_header)+TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_MAGIC($s2)
   ori   $t0,$t0,TUX64_BOOT_STAGE0_BOOT_HEADER_MAGIC_LO
   bne   $t0,$t1,tux64_boot_stage0_halt
   
   # calculate the header's checksum and verify it, this also gets the boot
   # header into cache so that rambus goes vroom vroom, also the next
   # instruction executes in the above branch delay slot
   lw    $t2,%lo(tux64_boot_stage1_boot_header)+TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_CHECKSUM($s2)
   addiu $t0,$s2,%lo(tux64_boot_stage1_boot_header)+TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA
   jal   tux64_boot_stage0_checksum_calculate_and_verify
   addiu $t1,$s2,%lo(tux64_boot_stage1_boot_header)+TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES

   # begin loading the stage-1 binary into memory
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1

   # read the stage-1 required memory and length
   lw    $k0,%lo(tux64_boot_stage1_boot_header)+TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_MEMORY_WORDS($s2)
   lw    $k1,%lo(tux64_boot_stage1_boot_header)+TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_LENGTH_WORDS($s2)

   # check if we have enough memory to load the stage-1 binary, done in terms of
   # words to avoid potential overflow
   srl   $t0,$a0,TUX64_BOOT_STAGE0_BOOT_HEADER_WORD_SIZE_POW2_EXPONENT
   addiu $t0,$t0,-(TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_REQUIRED_ADDITIONAL_MEMORY/TUX64_BOOT_STAGE0_BOOT_HEADER_WORD_SIZE)
   slt   $t1,$t0,$k0
   bne   $t1,$zero,tux64_boot_stage0_halt

   # convert the data word count and memory word count into byte counts
   sll   $k0,$k0,TUX64_BOOT_STAGE0_BOOT_HEADER_WORD_SIZE_POW2_EXPONENT # branch delay slot
   sll   $k1,$k1,TUX64_BOOT_STAGE0_BOOT_HEADER_WORD_SIZE_POW2_EXPONENT

   # copy the stage-1 bootloader into memory via PI DMA, no need to invalidate
   # cache since we only have the header in data cache, and since a data cache
   # line is 16 bytes, we will never have the stage-1 payload on the same cache
   # line as the header.  note that this concerns data cache.  if we were in
   # instruction cache, we would need to align to a 32-byte boundary.
   addiu $t2,$zero,TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_RDRAM_LO
   ori   $t3,$s1,TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_CARTRIDGE_ROM_LO
   addiu $t4,$k1,-1
   sw    $t2,TUX64_BOOT_STAGE0_ADDRESS_PI_DRAM_ADDR_LO($s0)
   sw    $t3,TUX64_BOOT_STAGE0_ADDRESS_PI_CART_ADDR_LO($s0)
   sw    $t4,TUX64_BOOT_STAGE0_ADDRESS_PI_WR_LEN_LO($s0)

   # calculate the total available memory for stage-1, reserving $a1
   subu  $a1,$a0,$k0

   # load the boot flags and stage-1 checksum, reserving $fp
   lw    $fp,%lo(tux64_boot_stage1_boot_header)+TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA_FLAGS($s0)

   # as a reminder, here are the current registers we care about:
   # $a0       - total memory
   # $a1       - available memory
   # $a2       - ique boolean
   # $s0       - PI MMIO registers base address
   # $s1       - cartridge ROM base address (physical address)
   # $s2       - cached RDRAM base address
   # $s3-$s7   - IPL2 arguments
   # $k1       - stage-1 length
   # $fp       - boot flags

   # calculate the checksum of the stage-1 binary and verify it's correct
   # unless the NO_CHECKSUM flag is set
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1

   # check if the NO_CHECKSUM flag is set, freeing $fp from this point forward
   andi  $fp,$fp,TUX64_BOOT_STAGE0_FLAG_NO_CHECKSUM
   bne   $fp,$zero,tux64_boot_stage0_start.skip_checksum_stage1

   # wait for the PI DMA operation to complete, then verify the checksum
   # branch delay slot.  we now free $k1.
   addiu $t0,$s2,TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_RDRAM_LO # branch delay slot

   tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_checksum:
      lw    $at,TUX64_BOOT_STAGE0_ADDRESS_PI_STATUS($s0)
      andi  $at,$at,TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY
      bne   $at,$zero,tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_checksum
   #tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_checksum

   addu  $t1,$t0,$k1 # branch delay slot
   lw    $t2,%lo(tux64_boot_stage1_boot_header)+TUX64_BOOT_STAGE0_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_CHECKSUM($s2)
   jal   tux64_boot_stage0_checksum_calculate_and_verify

   tux64_boot_stage0_start.skip_checksum_stage1:

   # this is now our current register allocation:
   # $a0       - total memory
   # $a1       - available memory
   # $a2       - ique boolean
   # $s0       - PI MMIO registers base address
   # $s1       - cartridge ROM base address (physical address)
   # $s2       - cached RDRAM base address
   # $s3-$s7   - IPL2 arguments
   # $fp       - boot flags

   # send the PIF terminate boot process command.  we use a dummy load
   # instruction to block the CPU until the SI bus isn't busy.
   lui   $k0,TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_HI # branch delay slot
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_PIF_TERMINATE_BOOT
   addiu $t2,$zero,TUX64_BOOT_STAGE0_PIF_COMMAND_TERMINATE_BOOT
   lw    $zero,TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_LO+0x3c($k0)
   sw    $t2,TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_LO+0x3c($k0)

   # calculate the stage-1 start address and prepare to start stage-1
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_START_STAGE1
   
   # wait for PI DMA, as we could reach here if we skipped the stage-1 checksum
   # without waiting on PI DMA to complete.
   tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_start:
      lw    $at,TUX64_BOOT_STAGE0_ADDRESS_PI_STATUS($s0)
      andi  $at,$at,TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY
      bne   $at,$zero,tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_start
   #tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_start

   # initialize the stack and start stage-1
   addiu $k0,$s2,TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_RDRAM_LO # branch delay slot
   jr    $k0
   addiu $sp,$s2,TUX64_BOOT_STAGE0_STAGE_1_STACK_SIZE
#tux64_boot_stage0_start

   # TODO: add multiple of these, one for each CIC revision.  only do this when
   # this code is finalized, as it takes long enough just to find one.
   .section .cic
tux64_boot_stage0_cic:
   .word 0x00013257
   .word 0x004930c9
#tux64_boot_stage0_cic

