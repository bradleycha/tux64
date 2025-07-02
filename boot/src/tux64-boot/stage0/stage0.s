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

.set noreorder
.set noat
.set nomacro

.equ TUX64_BOOT_STAGE0_COP0_REGISTER_TAGLO,$28
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_TAGHI,$29

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

.equ TUX64_BOOT_STAGE0_ADDRESS_MI_VERSION_LO,TUX64_BOOT_STAGE0_ADDRESS_MI_LO+0x0004

.equ TUX64_BOOT_STAGE0_ADDRESS_PI_HI,0xa460
.equ TUX64_BOOT_STAGE0_ADDRESS_PI_LO,0x0000

.equ TUX64_BOOT_STAGE0_ADDRESS_PI_DRAM_ADDR_LO,0x0000
.equ TUX64_BOOT_STAGE0_ADDRESS_PI_CART_ADDR_LO,0x0004
.equ TUX64_BOOT_STAGE0_ADDRESS_PI_WR_LEN_LO,0x000c

.equ TUX64_BOOT_STAGE0_ADDRESS_RI_HI,0xa470
.equ TUX64_BOOT_STAGE0_ADDRESS_RI_LO,0x0000

.equ TUX64_BOOT_STAGE0_ADDRESS_RI_SELECT_LO,TUX64_BOOT_STAGE0_ADDRESS_RI_LO+0x000c

.equ TUX64_BOOT_STAGE0_ADDRESS_CARTRIDGE_ROM_HI,0xb000
.equ TUX64_BOOT_STAGE0_ADDRESS_CARTRIDGE_ROM_LO,0x0000

.equ TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_HI,0xbfc0
.equ TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_LO,0x07c0

.equ TUX64_BOOT_STAGE0_MI_VERSION_IQUE_HI,0x0202
.equ TUX64_BOOT_STAGE0_MI_VERSION_IQUE_LO,0xb0b0

.equ TUX64_BOOT_STAGE0_PIF_COMMAND_TERMINATE_BOOT,0x0008

.equ TUX64_BOOT_STAGE0_1MIB_HI,0x0010

.equ TUX64_BOOT_STAGE0_STAGE_1_STACK_SIZE,0x1000

.equ TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES,0x0040
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_HI,0x1000
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_LO,0x1000

.equ TUX64_BOOT_STAGE0_HEADER_MAGIC_HI,0x5442 /* TB */
.equ TUX64_BOOT_STAGE0_HEADER_MAGIC_LO,0x484d /* HM */

.equ TUX64_BOOT_STAGE0_FLAG_NO_CHECKSUM,0x0001

.equ TUX64_BOOT_STAGE0_PAYLOAD_MAX_WORDS_STAGE1,1024

.equ TUX64_BOOT_STAGE0_STATUS_BYTES,8

.equ TUX64_BOOT_STAGE0_STATUS_ADDRESS_LO,TUX64_BOOT_STAGE0_ADDRESS_RSP_DMEM_LO+0x0ff8

.equ TUX64_BOOT_STAGE0_STATUS_HWORD0,0x5354 /* ST */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD1,0x4147 /* AG */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD2,0x4530 /* E0 */
.equ TUX64_BOOT_STAGE0_STATUS_HWORD3,0x3a00 /* :(null) */

.equ TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN,                '0'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_COP0_INITIALIZE,      '1'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_RDRAM_INITIALIZE,     '2'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_TOTAL_MEMORY,  '3'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_BOOT_HEADER,     '4'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CPU_CACHE_INITIALIZE, '5'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER,    '6'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1,          '7'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1,         '8'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_PIF_TERMINATE_BOOT,   '9'
.equ TUX64_BOOT_STAGE0_STATUS_CODE_START_STAGE1,         'a'

   .section .status
tux64_boot_stage0_status:
   # This is a chunk of memory placed at the end of RSP DMEM to show the current
   # boot status in memory to help debug early boot issues with memory
   # debuggers.  It will look like "STAGE0:c" when memory is displayed as ASCII
   # text, where 'c' is the current boot stage code.
   .skip TUX64_BOOT_STAGE0_STATUS_BYTES
#tux64_boot_stage0_status

   .section .boot_header
tux64_boot_stage0_boot_header:
   .skip TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES
#tux64_boot_stage0_boot_header

   .section .text
tux64_boot_stage0_status_code_write:
   lui   $t0,TUX64_BOOT_STAGE0_STATUS_HWORD2
   ori   $t0,$t0,TUX64_BOOT_STAGE0_STATUS_HWORD3
   or    $t0,$t0,$a0
   jr    $ra
   sw    $t0,TUX64_BOOT_STAGE0_STATUS_ADDRESS_LO+0x04($gp)
#tux64_boot_stage0_status_code_write

   .section .text
tux64_boot_stage0_halt:
   b     tux64_boot_stage0_halt
   nop
#tux64_boot_stage0_halt

   .section .start
   .global tux64_boot_stage0_start
tux64_boot_stage0_start:
   # reserve $gp for the start of RSP DMEM, used everywhere to cut down on code
   lui   $gp,TUX64_BOOT_STAGE0_ADDRESS_RSP_DMEM_HI

   # initialize status identifier and code
   lui   $s0,TUX64_BOOT_STAGE0_STATUS_HWORD0
   ori   $s0,$s0,TUX64_BOOT_STAGE0_STATUS_HWORD1
   sw    $s0,TUX64_BOOT_STAGE0_STATUS_ADDRESS_LO+0x00($gp)
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN

   # initialize COP0 registers
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_COP0_INITIALIZE
   # TODO: implement

   # begin initializing RDRAM
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_RDRAM_INITIALIZE

   # if we're on ique, there's no RDRAM to initialize, so we skip it
   lui   $s0,TUX64_BOOT_STAGE0_ADDRESS_MI_HI
   lui   $s1,TUX64_BOOT_STAGE0_MI_VERSION_IQUE_HI
   lw    $s2,TUX64_BOOT_STAGE0_ADDRESS_MI_VERSION_LO($s0)
   ori   $s1,$s1,TUX64_BOOT_STAGE0_MI_VERSION_IQUE_LO
   beq   $s1,$s2,tux64_boot_stage0_start.skip_rdram_initialization

   # reserve $s3 for the RI base address until the end of RDRAM initialization,
   # using the branch delay slot above
   lui   $s3,TUX64_BOOT_STAGE0_ADDRESS_RI_HI

   # check if RDRAM has already been initialized
   lw    $s4,TUX64_BOOT_STAGE0_ADDRESS_RI_SELECT_LO($s3)
   andi  $s4,$s4,0x00ff # mask out undefined bits
   bne   $s4,$zero,tux64_boot_stage0_start.skip_rdram_initialization

   # TODO: implement actual initialization, it seems Ares doesn't emulate
   # RDRAM properly as its initialized for us, so for now we can skip this, but
   # to boot on real hardware we're going to have to revisit this.
   nop
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
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_TOTAL_MEMORY

   addiu $s0,$zero,0
   lui   $s1,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_UNCACHED_HI
   lui   $s2,TUX64_BOOT_STAGE0_1MIB_HI
   lui   $s3,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_UNCACHED_HI + (TUX64_BOOT_STAGE0_1MIB_HI * 8)
   tux64_boot_stage0_start.detect_total_memory:
      ld    $s4,0($s1)     # load current value
      nor   $s5,$s4,$zero  # flips all bits
      sd    $s5,0($s1)     # store the flipped bits
      ld    $s6,0($s1)     # reload to see what was stored
      sd    $s4,0($s1)     # restore the old value

      # if the values differ, then either the memory chip is bad or we're at the
      # end of available memory.  exit the loop.
      bne   $s5,$s6,tux64_boot_stage0_start.detect_total_memory.exit

      # advance the memory pointer using the branch delay slot
      add   $s1,$s1,$s2

      # the maximum amount of possible memory is 8MiB.  if we're not past 8MiB
      # of memory, continue looping
      bne   $s1,$s3,tux64_boot_stage0_start.detect_total_memory

      # increment the total detected memory using the branch delay slot
      add   $s0,$s0,$s2
   #tux64_boot_stage0_start.detect_total_memory
   tux64_boot_stage0_start.detect_total_memory.exit:

   # from this point forward, $s0 is reserved for the total system memory, in
   # bytes until stage-1 begins.  this will be passed as an argument to stage-1
   # to avoid using memory unnecessarily.

   # load the boot header into RDRAM via PI DMA
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_BOOT_HEADER

   # reserve $s1 for the cartridge ROM base address, saves a couple of instructions
   lui   $s1,TUX64_BOOT_STAGE0_ADDRESS_CARTRIDGE_ROM_HI

   # dummy read to wait for the PI to not be busy
   lw    $zero,0($s1)

   # initiate the transfer in the background, we don't need to invalidate cache
   # as we're initializing it below anyways.
   lui   $s2,TUX64_BOOT_STAGE0_ADDRESS_PI_HI
   lui   $s4,TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_HI
   addiu $s3,$zero,(TUX64_BOOT_STAGE0_STAGE_1_STACK_SIZE + TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES)
   addiu $s5,$zero,(TUX64_BOOT_STAGE0_BOOT_HEADER_BYTES - 1)
   ori   $s4,$s4,TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_LO
   sw    $s3,TUX64_BOOT_STAGE0_ADDRESS_PI_DRAM_ADDR_LO($s2)
   sw    $s4,TUX64_BOOT_STAGE0_ADDRESS_PI_CART_ADDR_LO($s2)
   sw    $s5,TUX64_BOOT_STAGE0_ADDRESS_PI_WR_LEN_LO($s2)

   # initialize the CPU caches, setting each line to 'invalid', continue
   # loading the boot header in the background
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_CPU_CACHE_INITIALIZE
   lui   $s3,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_CACHED_HI
   lui   $s4,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_CACHED_HI
   lui   $s2,TUX64_BOOT_STAGE0_ADDRESS_RDRAM_CACHED_HI
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_TAGLO # write 'invalid' tag
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_TAGHI # must be zero, undefined on boot
   addiu $s3,$s3,(TUX64_BOOT_STAGE0_ICACHE_BYTES_PER_LINE * TUX64_BOOT_STAGE0_ICACHE_LINE_COUNT)
   addiu $s4,$s4,(TUX64_BOOT_STAGE0_DCACHE_BYTES_PER_LINE * TUX64_BOOT_STAGE0_DCACHE_LINE_COUNT)
   tux64_boot_stage0_start.initialize_cache:
      addiu $s3,$s3,-TUX64_BOOT_STAGE0_ICACHE_BYTES_PER_LINE
      addiu $s4,$s4,-TUX64_BOOT_STAGE0_DCACHE_BYTES_PER_LINE
      cache (TUX64_BOOT_STAGE0_CACHE_TYPE_ICACHE | TUX64_BOOT_STAGE0_CACHE_INDEX_STORE_TAG),0($s3)
      bne   $s2,$s3,tux64_boot_stage0_start.initialize_cache
      cache (TUX64_BOOT_STAGE0_CACHE_TYPE_DCACHE | TUX64_BOOT_STAGE0_CACHE_INDEX_STORE_TAG),0($s4)
   #tux64_boot_stage0_start.initialize_cache

   # we will now reserve $s2 for the cached RDRAM base address.  remember, we
   # still have $s0 reserved for the total memory, and $s1 for the cartridge ROM
   # base address

   # calculate the checksum of the boot header and verify it matches what's
   # given by the header
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER

   # another dummy read to make sure the boot header DMA completed
   lw    $zero,0($s1)

   # TODO: calculate checksum and compare

   # load the stage-1 binary into memory
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1
   # TODO: implement

   # calculate the checksum of the stage-1 binary and verify it's correct
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1
   # TODO: implement

   # send the PIF terminate boot process command.  we use a dummy load
   # instruction to block the CPU until the SI bus isn't busy.
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_PIF_TERMINATE_BOOT
   lui   $s3,TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_HI
   addiu $s4,$zero,TUX64_BOOT_STAGE0_PIF_COMMAND_TERMINATE_BOOT
   lw    $zero,TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_LO+0x3c($s3)
   sw    $s4,TUX64_BOOT_STAGE0_ADDRESS_PIF_RAM_LO+0x3c($s3)

   # jump to stage-1 start address
   jal   tux64_boot_stage0_status_code_write
   addiu $a0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_START_STAGE1
   # TODO: implement
   b     tux64_boot_stage0_halt
   nop
#tux64_boot_stage0_start

   # TODO: add multiple of these, one for each CIC revision.  only do this when
   # this code is finalized, as it takes long enough just to find one.
   .section .cic
tux64_boot_stage0_cic:
   # TODO: recalculate these once we finish developing stage-0
   .word 0x00000000
   .word 0x00000000
#tux64_boot_stage0_cic

