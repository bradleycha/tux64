/*----------------------------------------------------------------------------*/
/*                       Copyright (c) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage0/stage0.S - Stage-0 bootloader code and CIC      */
/*    data.                                                                   */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* The stage-0 bootloader runs from RSP DMEM + 0x40, directly after the ROM   */
/* header.  At this point, RDRAM is uninitialized, and the CPU's cache is     */
/* uninitialized.  Our goal is to do the following:                           */
/*                                                                            */
/*  * Initialize COP0 registers to a consistent state                         */
/*  * Initialize RDRAM                                                        */
/*  * Initialze the CPU caches                                                */
/*  * Load the boot header into RDRAM via PI DMA                              */
/*  * Verify the boot header's checksum                                       */
/*  * Load the stage-1 binary into RDRAM via PI DMA                           */
/*  * Verify the stage-1 binary's checksum if boot flag NO_CHECKSUM isn't set */
/*  * Move the stack into RDRAM                                               */
/*  * Jump to the stage-1 start address                                       */
/*                                                                            */
/* Now, the reason we're in assembly: DRM bullshit, yay!  More specifically,  */
/* IPL2 verifies the checksum of the "IPL3" with the PIF, which will in turn  */
/* halt the CPU if the checksum computed doesn't match the cartridge CIC's    */
/* matching checksum.  This was done by Nintendo as a method to troll people  */
/* attempting to write their own code in replacement of the IPL3.  However,   */
/* there is a workaround.  We can use ipl3hasher(-new) to brute-force 8 bytes */
/* to place at the end of the stage-0 binary to force the checksum of our     */
/* code to match the stock IPL3's checksum.  The upside is it does work, and  */
/* we can write fully custom code with 0% of Nintendo's shit...but the        */
/* downside is we need to recompute the bytes every time we change the code.  */
/* To prevent pissing off non-developers, we write the entire stage-0 in      */
/* assembly so we can get 100% consistent machine code, then simply           */
/* distribute the brute-forced bytes with the source code so only developers  */
/* modifying stage-0 have to compute the checksum bytes.  For the rest of the */
/* boot process, we have full control over the system, so we can use C        */
/* instead of assembly.                                                       */
/*                                                                            */
/* Note #1: due to the way the SysAD bus and PI+SI+RCP handles reads and      */
/* writes to their respective MMIO address spaces, we need to be careful when */
/* accessing memory.  For RSP IMEM/DMEM, all read sizes except 64-bit are     */
/* allowed, but only 32-bit writes are valid.  For PI+SI-mapped data,         */
/* only 32-bit reads are allowed, and only 32-bit writes are allowed when the */
/* I/O busy/DMA busy bits aren't set in the PI_STATUS/SI_STATUS registers.    */
/* Accesses of all sizes are allowed in RDRAM once its initialized.           */
/*                                                                            */ 
/* Note #2: we disable assembler reordering of instructions, use of $at, and  */
/* assembler macros. this is done so we have 100% consistent machine code,    */
/* ideally no matter what assembler or assembler flags are in use.  do        */
/* do remember, though, that branch delay slots lead to 1 instruction after   */
/* most branch instructions to be executed.  This does make it even more      */
/* difficult to program, but just remember...no pain, no gain!                */
/*                                                                            */
/* Note #3: For optimization, we use a custom calling convention that goes    */
/* like this:                                                                 */
/*                                                                            */
/* $t0-$t3:          arguments, volatile                                      */
/* $t4-$t7, $at:     volatile                                                 */
/* $v0-$v1:          return value, volatile                                   */
/* $ra:              return address, volatile                                 */
/* everything else:  non-volatile                                             */
/*----------------------------------------------------------------------------*/

#include <tux64/platform/mips/vr4300/cop0.h>
#include <tux64/platform/mips/vr4300/cache.h>
#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/n64/mi.h>
#include <tux64/platform/mips/n64/pi.h>
#include <tux64/platform/mips/n64/ri.h>
#include <tux64/platform/mips/n64/rdram.h>
#include <tux64/platform/mips/n64/boot.h>

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/status.h"
#include "tux64-boot/stage1/stack.h"
#include "tux64-boot/stage1/memory.h"

.set noreorder
.set noat
.set nomacro

/* need to do this because of dumb shit with how preprocessor is evaluated. */
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_COUNT,$TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_COUNT
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_COMPARE,$TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_COMPARE
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_STATUS,$TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_STATUS
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_CAUSE,$TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CAUSE
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_TAGLO,$TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_TAGLO
.equ TUX64_BOOT_STAGE0_COP0_REGISTER_TAGHI,$TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_TAGHI

.equ TUX64_BOOT_STAGE0_MI_MODE_REPEAT_16,0x010f
.equ TUX64_BOOT_STAGE0_MI_VERSION_IQUE_HI,0x0202
.equ TUX64_BOOT_STAGE0_MI_VERSION_IQUE_LO,0xb0b0

.equ TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY,(TUX64_PLATFORM_MIPS_N64_PI_STATUS_BIT_DMA_BUSY | TUX64_PLATFORM_MIPS_N64_PI_STATUS_BIT_IO_BUSY)

.equ TUX64_BOOT_STAGE0_RI_CONFIG_AUTO_CURRENT_CALIBRATION,0x0040
.equ TUX64_BOOT_STAGE0_RI_RX_TX_TIMINGS,0x0014
.equ TUX64_BOOT_STAGE0_RI_MODE_STANDARD,0x000e
.equ TUX64_BOOT_STAGE0_RI_REFRESH_CLEANDELAY,(52)
.equ TUX64_BOOT_STAGE0_RI_REFRESH_DIRTYDELAY,(54 << 8)
.equ TUX64_BOOT_STAGE0_RI_REFRESH_BASE,(TUX64_PLATFORM_MIPS_N64_RI_REFRESH_BIT_OPTIMIZE | TUX64_PLATFORM_MIPS_N64_RI_REFRESH_BIT_ENABLE | TUX64_BOOT_STAGE0_RI_REFRESH_CLEANDELAY | TUX64_BOOT_STAGE0_RI_REFRESH_DIRTYDELAY)

.equ TUX64_BOOT_STAGE0_RDRAM_DELAY_TIMING,0x18082838
.equ TUX64_BOOT_STAGE0_RDRAM_MODE_INITIAL,0x44000000
.equ TUX64_BOOT_STAGE0_RDRAM_MODE_CURRENT_CALIBRATE_MANUAL,(1 << 1) | (1 << 2) | (1 << 6)
.equ TUX64_BOOT_STAGE0_RDRAM_MODE_CURRENT_CALIBRATE_AUTOMATIC,(1 << 1) | (1 << 2) | (1 << 6) | (1 << 7)
.equ TUX64_BOOT_STAGE0_RDRAM_DEVICEID_INITIAL,511
.equ TUX64_BOOT_STAGE0_RDRAM_DEVICEID_INITIAL_ENCODED,0xfc800300 /* device ID 511 */
.equ TUX64_BOOT_STAGE0_RDRAM_DEVICEID_INVALID_ENCODED,0xf4800300 /* device ID 509 */
.equ TUX64_BOOT_STAGE0_RDRAM_RAS_INTERVAL_DEFAULT,0x101c0a04
.equ TUX64_BOOT_STAGE0_RDRAM_RAS_INTERVAL_UNKNOWN,0x080c1204

.equ TUX64_BOOT_STAGE0_1MIB_HI,0x0010

.equ TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_RDRAM_LO,TUX64_BOOT_STAGE1_STACK_BYTES
.equ TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_LO,0x1000

.equ TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_CARTRIDGE_ROM_LO,TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_LO+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES
.equ TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_RDRAM_LO,TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_RDRAM_LO+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES_ALIGN16

.equ TUX64_BOOT_STAGE0_STATUS_WORD_HI,TUX64_BOOT_STATUS_MAGIC_HI                 /* STAG */
.equ TUX64_BOOT_STAGE0_STATUS_WORD_LO,TUX64_BOOT_STATUS_MAGIC_LO | ('0' << 16)   /* E0:x */

/* we store the upper and lower byte of the status code together to save on */
/* an 'or' instruction for free in tux64_boot_stage0_status_code_write(). */
#define TUX64_BOOT_STAGE0_STATUS_CODE(code)\
   ((TUX64_BOOT_STAGE0_STATUS_WORD_LO & 0xffff) | code)

.equ TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN,                         TUX64_BOOT_STAGE0_STATUS_CODE('A')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_COP0_INITIALIZE,               TUX64_BOOT_STAGE0_STATUS_CODE('B')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_HARDWARE_INFORMATION,   TUX64_BOOT_STAGE0_STATUS_CODE('C')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_RDRAM_INITIALIZE,              TUX64_BOOT_STAGE0_STATUS_CODE('D')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_TOTAL_MEMORY,           TUX64_BOOT_STAGE0_STATUS_CODE('E')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_BOOT_HEADER,              TUX64_BOOT_STAGE0_STATUS_CODE('F')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CPU_CACHE_INITIALIZE,          TUX64_BOOT_STAGE0_STATUS_CODE('G')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER,             TUX64_BOOT_STAGE0_STATUS_CODE('H')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1,                   TUX64_BOOT_STAGE0_STATUS_CODE('I')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1,                  TUX64_BOOT_STAGE0_STATUS_CODE('J')
.equ TUX64_BOOT_STAGE0_STATUS_CODE_START_STAGE1,                  TUX64_BOOT_STAGE0_STATUS_CODE('K')

/* defined in stage1/boot-header.ld, included in stage0/stage0.ld */
.extern tux64_boot_stage1_boot_header

   .section .tux64_boot.status
tux64_boot_stage0_status:
   /* This is a chunk of memory placed at the end of RSP DMEM to show the */
   /* current boot status in memory to help debug early boot issues with */
   /* memory debuggers.  It will look like "STAGE0:c" when memory is */
   /* displayed as ASCII text, where 'c' is the current boot stage code. */
   .skip TUX64_BOOT_STATUS_BYTES
/*tux64_boot_stage0_status*/

   .section .rodata
tux64_boot_stage0_float.1div80f:
   .word 0x3c4ccccd /* 1.0f / 80.0f */
tux64_boot_stage0_float.2pt2f:
   .word 0x400ccccd /* 2.2f */
tux64_boot_stage0_float.0pt5f:
   .word 0x3f000000 /* 0.5f */

   .section .text
tux64_boot_stage0_halt:
   /* set the processor into low-power mode so we don't waste power before */
   /* entering the infinite spinlock.  we don't care about updating RDRAM */
   /* refresh rate or anything since we don't do anything else from this */
   /* point.  memory will be re-initialized on next startup, so memory */
   /* decaying away isn't our problem. */

   /* bogus read to to uncached memory to force SysAD to be inactive before */
   /* enabling low-power mode, per the processor's documentation */
   lw    $zero,0($gp)

   /* set the RP bit in the COP0 Status register */
   lui   $t1,%hi(TUX64_PLATFORM_MIPS_VR4300_COP0_STATUS_BIT_RP)
   mfc0  $t0,TUX64_BOOT_STAGE0_COP0_REGISTER_STATUS
   or    $t0,$t0,$t1
   mtc0  $t0,TUX64_BOOT_STAGE0_COP0_REGISTER_STATUS

   /* we now deliberately make an invalid 64-bit read to RSP DMEM.  this will */
   /* cause the CPU to hang indefinitely waiting for a second 32-bit word to */
   /* be returned, which will never happen.  this saves an instruction over */
   /* a basic spinlock and might save more power as well */
   ld    $zero,0($gp)
/*tux64_boot_stage0_halt*/

#if TUX64_BOOT_CONFIG_DUMP_TO_SRAM
/*----------------------------------------------------------------------------*/

   .section .text
tux64_boot_stage0_sram_write_word:
   /* writes a word to an offset in cartridge SRAM.                           */
   /*                                                                         */
   /* $t0 - 32-bit word to write to the offset pointed at by $t0.             */
   /* $t1 - address offset into SRAM.                                         */
   lui   $t3,%hi(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_STATUS)

   tux64_boot_stage0_sram_write_word.pi_io_dma_spinlock:
      lw    $at,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_STATUS)($t3)
      andi  $at,$at,TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY
      bne   $at,$zero,tux64_boot_stage0_sram_write_word.pi_io_dma_spinlock
   /*tux64_boot_stage0_sram_write_word.pi_io_dma_spinlock*/

   lui   $t2,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_CARTRIDGE_RAM) /* branch delay slot */
   addu  $t2,$t2,$t1
   jr    $ra
   sw    $t0,%lo(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_CARTRIDGE_RAM)($t2) /* branch delay slot */
/*tux64_boot_stage0_sram_write_word*/

   .section .text
tux64_boot_stage0_dump_to_sram:
   /* backup the register file, careful not to touch anything. */
   sw    $0, -128($sp)
   sw    $1, -124($sp)
   sw    $2, -120($sp)
   sw    $3, -116($sp)
   sw    $4, -112($sp)
   sw    $5, -108($sp)
   sw    $6, -104($sp)
   sw    $7, -100($sp)
   sw    $8, -96($sp)
   sw    $9, -92($sp)
   sw    $10,-88($sp)
   sw    $11,-84($sp)
   sw    $12,-80($sp)
   sw    $13,-76($sp)
   sw    $14,-72($sp)
   sw    $15,-68($sp)
   sw    $16,-64($sp)
   sw    $17,-60($sp)
   sw    $18,-56($sp)
   sw    $19,-52($sp)
   sw    $20,-48($sp)
   sw    $21,-44($sp)
   sw    $22,-40($sp)
   sw    $23,-36($sp)
   sw    $24,-32($sp)
   sw    $25,-28($sp)
   sw    $26,-24($sp)
   sw    $27,-20($sp)
   sw    $28,-16($sp)
   sw    $29,-12($sp)
   sw    $30,-8($sp)
   sw    $31,-4($sp)
   addiu $sp,$sp,-128

   /* first, write the status code out. */
   lui   $s0,%hi(TUX64_BOOT_STAGE0_STATUS_WORD_HI)
   addiu $t1,$zero,0
   ori   $s0,$s0,%lo(TUX64_BOOT_STAGE0_STATUS_WORD_HI)
   jal   tux64_boot_stage0_sram_write_word
   addiu $t0,$s0,0
   addiu $t1,$zero,8
   jal   tux64_boot_stage0_sram_write_word
   addiu $t0,$s0,0
   lw    $t0,%lo(tux64_boot_stage0_status)+0x04($gp)
   jal   tux64_boot_stage0_sram_write_word
   addiu $t1,$zero,12

   /* now dump the entire register file, stored on the stack. */
   addiu $s0,$sp,0    /* iter_reg  */
   addiu $s1,$zero,16 /* iter_sram */
   addiu $s2,$zero,32 /* i         */
   tux64_boot_stage0_dump_to_sram.dump_register:
      /* load the current register and write to sram */
      lw    $t0,0($s0)
      jal   tux64_boot_stage0_sram_write_word
      addiu $t1,$s1,0

      /* move to next register */
      addiu $s2,$s2,-1
      addiu $s1,$s1,4
      bne   $s2,$zero,tux64_boot_stage0_dump_to_sram.dump_register
      addiu $s0,$s0,4 /* branch delay slot */
   /*tux64_boot_stage0_dump_to_sram.dump_register*/

   /* now restore the register file and return */
   addiu $sp,$sp,128
   lw    $31,-4($sp)
   lw    $30,-8($sp)
   lw    $29,-12($sp)
   lw    $28,-16($sp)
   lw    $27,-20($sp)
   lw    $26,-24($sp)
   lw    $25,-28($sp)
   lw    $24,-32($sp)
   lw    $23,-36($sp)
   lw    $22,-40($sp)
   lw    $21,-44($sp)
   lw    $20,-48($sp)
   lw    $19,-52($sp)
   lw    $18,-56($sp)
   lw    $17,-60($sp)
   lw    $16,-64($sp)
   lw    $15,-68($sp)
   lw    $14,-72($sp)
   lw    $13,-76($sp)
   lw    $12,-80($sp)
   lw    $11,-84($sp)
   lw    $10,-88($sp)
   lw    $9, -92($sp)
   lw    $8, -96($sp)
   lw    $7, -100($sp)
   lw    $6, -104($sp)
   lw    $5, -108($sp)
   lw    $4, -112($sp)
   lw    $3, -116($sp)
   lw    $2, -120($sp)
   lw    $1, -124($sp)
   jr    $ra
   lw    $0, -128($sp)
/*tux64_boot_stage0_dump_to_sram*/

.macro TUX64_BOOT_STAGE0_DUMP_TO_SRAM
   nop
   jal tux64_boot_stage0_dump_to_sram
   nop
.endm

/*----------------------------------------------------------------------------*/
#else /* TUX64_BOOT_CONFIG_DUMP_TO_SRAM */
/*----------------------------------------------------------------------------*/

.macro TUX64_BOOT_STAGE0_DUMP_TO_SRAM
.endm

/*----------------------------------------------------------------------------*/
#endif /* TUX64_BOOT_CONFIG_DUMP_TO_SRAM */

   .section .text
tux64_boot_stage0_status_code_write:
   /* $t0 contains the low 16 bits or'd with the status code already, so we */
   /* only need to load the upper 2 bytes. */
   lui   $t1,%hi(TUX64_BOOT_STAGE0_STATUS_WORD_LO)
   or    $t0,$t0,$t1
#if TUX64_BOOT_CONFIG_DUMP_TO_SRAM
   sw    $t0,%lo(tux64_boot_stage0_status)+0x04($gp)
   b     tux64_boot_stage0_sram_write_word
   addiu $t1,$zero,4
#else /* TUX64_BOOT_CONFIG_DUMP_TO_SRAM */
   jr    $ra
   sw    $t0,%lo(tux64_boot_stage0_status)+0x04($gp)
#endif /* TUX64_BOOT_CONFIG_DUMP_TO_SRAM */
/*tux64_boot_stage0_status_code_write*/

   .section .text
tux64_boot_stage0_checksum_calculate_and_verify:
   /* calculates a checksum and verifies it's correct, halting if it's not    */
   /* correct and returning if it is correct.  data must be word-aligned.     */
   /*                                                                         */ 
   /* $t0 - data start ptr                                                    */
   /* $t1 - data end ptr                                                      */
   /* $t2 - expected checksum                                                 */

   addiu $t3,$zero,0 /* sum_hi */
   addiu $t4,$zero,0 /* sum_lo */
   tux64_boot_stage0_checksum_calculate_and_verify.digest_word:
      lw    $t5,0($t0)
      addu  $t3,$t3,$t5
      addu  $t4,$t4,$t3
      addiu $t0,$t0,4
      bne   $t0,$t1,tux64_boot_stage0_checksum_calculate_and_verify.digest_word
   /*tux64_boot_stage0_checksum_calculate_and_verify.digest_word*/

   subu  $t6,$t4,$t3 /* executes in branch delay slot of loop above */
   bne   $t6,$t2,tux64_boot_stage0_halt

   nop
   jr    $ra
   nop
/*tux64_boot_stage0_checksum_calculate_and_verify*/

   .section .text
tux64_boot_stage0_rdram_wait:
   addiu $t0,$zero,0x0100
   tux64_boot_stage0_rdram_wait.loop:
      addiu $t0,$t0,-1
      bne   $t0,$zero,tux64_boot_stage0_rdram_wait.loop
      nop
   /*tux64_boot_stage0_rdram_wait.loop*/

   jr    $ra
   nop
/*tux64_boot_stage0_rdram_wait*/

   .section .start
   .global tux64_boot_stage0_start
tux64_boot_stage0_start:
   /* for starters, we have the following reserved registers from IPL2 which  */
   /* will later be passed to stage-1:                                        */
   /* $s3 - ROM type (stock/64DD)                                             */
   /* $s4 - video standard (NTSC, PAL, M-PAL)                                 */
   /* $s5 - reset type                                                        */
   /* $s6 - ROM CIC seed                                                      */
   /* $s7 - PIF ROM version                                                   */

   /* reserve $gp for the start of RSP DMEM, used everywhere to cut down on */
   /* code */
   lui   $gp,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM)

   /* initialize status identifier and code */
   lui   $t2,%hi(TUX64_BOOT_STAGE0_STATUS_WORD_HI)
   ori   $t2,$t2,%lo(TUX64_BOOT_STAGE0_STATUS_WORD_HI)
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_BEGIN
   jal   tux64_boot_stage0_status_code_write
   sw    $t2,%lo(tux64_boot_stage0_status)+0x00($gp)

   /* begin initializing COP0 registers */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_COP0_INITIALIZE

   /* i'm not entirely sure why everyone initializes these specific */
   /* registers.  it seems it's to clear exceptions and some timer thing from */
   /* a reset? nobody anywhere explains why, so it looks like i'll be copying */
   /* the person who copied the person who copied the person who wrote the */
   /* official IPL3.  */
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_CAUSE
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_COUNT
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_COMPARE

   /* begin detecting hardware information */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_HARDWARE_INFORMATION

   /* reserve $fp for the MI base address until the end of RDRAM */
   /* initialization */
   lui   $fp,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI)

   /* detect the hardware revision, which is used in RDRAM initialization and */
   /* to detect ique support.  we then reserve $s0 for the hardware revision */
   /* until we detect HW1/HW2 RDRAM and $s2 for the boolean if we're on ique */
   /* or not.  we then skip RDRAM initialization if we're on ique, as there's */
   /* no RDRAM to initialize. */
   lui   $t0,TUX64_BOOT_STAGE0_MI_VERSION_IQUE_HI
   lw    $t1,%lo(TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_VERSION)($fp)
   ori   $t0,$t0,TUX64_BOOT_STAGE0_MI_VERSION_IQUE_LO
   xor   $s2,$t0,$t1
   andi  $s0,$t1,0x00ff
   beq   $t0,$t1,tux64_boot_stage0_start.skip_rdram_initialization
   sltiu $s2,$s2,1 /* branch delay slot */

   /* begin initializing RDRAM */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_RDRAM_INITIALIZE

   /* TODO: this does not work.  something is messed up with the RDRAM init */
   /* code.  theoretically this is just a manually compiled version of the */
   /* libdragon rdram.c, but when run on real hardware, it fails to detect */
   /* and configure any memory chips, thus we always boot with no memory */
   /* configured.  this will be extra fun debugging because not only is it */
   /* highly optimized assembly for a complicated process, but we also have */
   /* no easy way to debug this since ares doesn't accurately emulate rdram.  */
   /* thus, our only option is to either write information to sram, or attach */
   /* a logic analyzer to the sysad bus on a real N64 and watch the reads and */
   /* writes from the MI, RI, and RDRAM MMIO registers. */

   /* reserve $s1 for the RI base address until the end of RDRAM */
   /* initialization */
   lui   $s1,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI)

   /* check if RDRAM has already been initialized */
   lw    $t0,%lo(TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_SELECT)($s1)
   andi  $t0,$t0,0x00ff /* mask out undefined bits */
   bne   $t0,$zero,tux64_boot_stage0_start.skip_rdram_initialization

   /* i'll admit, the RDRAM code below is mostly referenced from libdragon, */
   /* as this shit is too complex to free-style. */

   /* perform current calibration */
   addiu $t1,$zero,TUX64_BOOT_STAGE0_RI_CONFIG_AUTO_CURRENT_CALIBRATION /* branch delay slot */
   jal   tux64_boot_stage0_rdram_wait
   sw    $t1,%lo(TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_CONFIG)($s1) /* branch delay slot */
   sw    $zero,%lo(TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_CURRENT_LOAD)($s1)

   /* configure rx/tx signal timings */
   addiu $t0,$zero,TUX64_BOOT_STAGE0_RI_RX_TX_TIMINGS
   sw    $t0,%lo(TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_SELECT)($s1)

   /* reset all the chips */
   jal   tux64_boot_stage0_rdram_wait
   sw    $zero,%lo(TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_MODE)($s1) /* RI_MODE = reset, branch delay slot */
   addiu $t0,$zero,TUX64_BOOT_STAGE0_RI_MODE_STANDARD
   jal   tux64_boot_stage0_rdram_wait
   sw    $t0,%lo(TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_MODE)($s1) /* branch delay slot */

   /* reserve $a2 for the spacing between RDRAM registers, and $k0 for the */
   /* offset to access rdram registers for the initial chip ID.  HW1 rdram is */
   /* spaced out by 0x200 bytes, and everything else is spaced out by 0x400  */
   /* bytes.  note that we double $a2 because in the chip initialization  */
   /* loop, we access only increments of 2 chips at a time, so we save an */
   /* instruction by doubling the constant here. */
   lui   $k0,%hi(TUX64_BOOT_STAGE0_RDRAM_DEVICEID_INITIAL * 0x200)
   addiu $t0,$zero,1
   ori   $k0,$k0,%lo(TUX64_BOOT_STAGE0_RDRAM_DEVICEID_INITIAL * 0x200)
   beq   $t0,$s0,tux64_boot_stage0_start.rdram_hw1
   addiu $a2,$zero,(0x200 * 2) /* branch delay slot */
   sll   $k0,$k0,1
   sll   $a2,$a2,1
   tux64_boot_stage0_start.rdram_hw1:

   /* reserve $a3 for the base address of RDRAM registers.  this will be used */
   /* both to do broadcast writes as well as per-chip access.  we will also */
   /* use $t2 for broadcast writes we do shortly. */
   lui   $t2,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_REGISTERS_BROADCAST)
   lui   $a3,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_REGISTERS)

   /* initialize Delay register.  see libdragon/boot/rdram.c:142 for more */
   /* information. */
   lui   $t0,%hi(TUX64_BOOT_STAGE0_RDRAM_DELAY_TIMING)
   addiu $t1,$zero,TUX64_BOOT_STAGE0_MI_MODE_REPEAT_16
   ori   $t0,$t0,%lo(TUX64_BOOT_STAGE0_RDRAM_DELAY_TIMING)
   sw    $t1,%lo(TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_MODE)($fp)
   sw    $t0,(%lo(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_REGISTERS_BROADCAST) + TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DELAY)($t2)

   /* initialize the device IDs for every chip */
   lui   $t3,%hi(TUX64_BOOT_STAGE0_RDRAM_DEVICEID_INITIAL_ENCODED)
   ori   $t3,$t3,%lo(TUX64_BOOT_STAGE0_RDRAM_DEVICEID_INITIAL_ENCODED)
   lui   $t4,%hi(TUX64_BOOT_STAGE0_RDRAM_MODE_INITIAL) /* no low-order bits, so we only need lui */
   sw    $t3,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DEVICE_ID($t2)
   sw    $t4,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_MODE($t2)
   sw    $zero,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_REF_ROW($t2)

   /* compute the base address for the rdram registers for the initial chip */
   /* id and reserve $k0 for it until we finish initializing all chips. */
   addu  $k0,$k0,$a3

   /* reserve $t8 and $t9 for the SetUpper and ClearUpper bits for MI_MODE, */
   /* which are needed because odd-numbered RDRAM registers required */
   /* "Upper Mode".  */
   addiu $t8,$zero,TUX64_PLATFORM_MIPS_N64_MI_MODE_BIT_UPPER_SET
   addiu $t9,$zero,TUX64_PLATFORM_MIPS_N64_MI_MODE_BIT_UPPER_CLEAR

   /* start initializing all chips, reserving $a1 for chip_id until the end */
   /* of rdram initialization and $a3 for the address of the current chip's */
   /* rdram registers. */
   addiu $a1,$zero,0 /* chip_id */
   tux64_boot_stage0_start.rdram_initialize_chip:
      /* set the chip ID.  all this bit math shit was previously done */
      /* statically with the constants, but now we have no choice but to */
      /* compute it at runtime.  this combines the weird bit shifting with */
      /* byte swapping to little endian, leading to borderline undecipherable */
      /* code.  we also load a constant which will be used below for */
      /* pipelining. */
      andi  $t0,$a1,0x003f
      andi  $t1,$a1,0x0040
      andi  $t2,$a1,0x7f80
      sll   $t0,$t0,26
      sll   $t1,$t1,17
      sll   $t2,$t2,1
      or    $t0,$t0,$t1
      or    $t0,$t0,$t2
      lui   $t3,0x4600 /* used below, placed here for pipelining */
      sw    $t0,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DEVICE_ID($k0)

      TUX64_BOOT_STAGE0_DUMP_TO_SRAM

      /* turn the chip on by setting the following bits for the Mode register */
      /* and flipping endianess:                                              */
      /* X2 (bit 1)                                                           */
      /* PL (bit 2)                                                           */
      /* DE (bit 6)                                                           */
      sw    $t3,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_MODE($a3)

      /* check if the DE bit was actually set.  if not, there's no more chips */
      /* and we are done. */
      sw    $t8,%lo(TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_MODE)($fp)
      lw    $t0,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_MODE($a3)
      sw    $t9,%lo(TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_MODE)($fp)
      srl   $t0,25
      andi  $t0,1
      beq   $t0,$zero,tux64_boot_stage0_start.rdram_initialize_chip.exit

      /* now we start current calibration.  we first configure the chips */
      /* manually and see the lowest cc value which gives us full accuracy. */
      addiu $a0,$zero,0                                     /* $a0 = cc, also branch delay slot */
      mtc1  $zero,$f0                                       /* $f0 = weighted_sum               */
      mtc1  $zero,$f1                                       /* $f1 = prev_accuracy              */
      lwc1  $f2,%lo(tux64_boot_stage0_float.1div80f)($gp)   /* $f2 = 1.0f/80.0f                 */
      tux64_boot_stage0_start.rdram_initialize_chip.cc_manual:
         /* configure the chip for the current cc value */
         jal   tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_manual
         addiu $t0,$a0,0 /* branch delay slot */

         /* perform a memory test to measure accuracy at this cc value */
         sll      $t2,$a1,20
         daddiu   $t0,$zero,10  /* loop count */
         lui      $t6,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_UNCACHED)
         daddiu   $t1,$zero,0   /* total counted accurate bits */
         addu     $t6,$t6,$t2   /* memory test base address */
         tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.memory_test:
            /* write 8 "0xff" bytes to the start of the current chip's memory */
            addiu $t2,$zero,-1
            sw    $t2,0($t6)
            sw    $t2,4($t6)

            /* now read back one byte and count the number of set bits */
            lb    $t2,5($t6)
            andi  $t2,$t2,0xff
            tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.memory_test.popcnt:
               beq   $t2,$zero,tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.memory_test.popcnt.exit
               dsll  $t3,$t2,63
               dsrl  $t3,$t3,63
               daddu $t1,$t1,$t3
               b     tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.memory_test.popcnt
               srl   $t2,$t2,1 /* branch delay slot */
            /*tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.memory_test.popcnt*/
            tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.memory_test.popcnt.exit:

            daddiu   $t0,$t0,-1
            bne      $t0,$zero,tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.memory_test
         /*tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.memory_test*/

         /* calculate accuracy as a percentage and accumulate weighted sum, */
         /* with a thousand nops because of stupid pipeline details and */
         /* hardware bugs. */
         mtc1     $t1,$f3 /* branch delay slot */
         mtc1     $a0,$f5
         cvt.s.w  $f3,$f3
         cvt.s.w  $f5,$f5     /* $f4 = (float)cc       */
         mul.s    $f3,$f3,$f2 /* $f3 = (float)accuracy */
         nop
         nop
         sub.s    $f6,$f3,$f1
         nop
         mul.s    $f6,$f6,$f5
         nop
         nop
         add.s    $f0,$f0,$f6 /* $f0 += (accuracy - prev_accuracy) * cc */

         /* exit if we have full accuracy */
         addiu $t2,80
         beq   $t1,$t2,tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.exit

         /* store the current accuracy as prev_accuracy */
         mov.s    $f1,$f3 /* branch delay slot */

         /* move to the next cc value */
         addiu $t3,64
         addiu $a0,$a0,1
         bne   $a0,$t3,tux64_boot_stage0_start.rdram_initialize_chip.cc_manual
         nop /* branch delay slot */
      /*tux64_boot_stage0_start.rdram_initialize_chip.cc_manual*/
      tux64_boot_stage0_start.rdram_initialize_chip.cc_manual.exit:

      /* compute target_cc and invalidate the chip if it's zero, reserving */
      /* $a0 for target_cc until the end of current calibration. */
      lwc1        $f1,%lo(tux64_boot_stage0_float.2pt2f)($gp) /* $f1 = 2.2f */
      lwc1        $f2,%lo(tux64_boot_stage0_float.0pt5f)($gp) /* $f2 = 0.5f */
      mul.s       $f1,$f0,$f1
      nop
      nop
      add.s       $f1,$f1,$f2
      nop
      trunc.w.s   $f1,$f1
      mfc1        $a0,$f1

      /* check to make sure target_cc is valid */
      beq   $a0,$zero,tux64_boot_stage0_start.rdram_initialize_chip.err_exit

      /* we now begin automatic current calibration.  it's worth noting by    */
      /* this point we are desperate for non-volatile registers, as these are */
      /* the only non-volatiles left:                                         */
      /*                                                                      */
      /* $v1, $k1, $sp                                                        */
      /*                                                                      */ 
      /* luckily, it turns out to be just enough.                             */
      addiu $v1,$zero,0 /* $v1 = minerr, branch delay slot */
      addiu $k1,$zero,0 /* $k1 = autocc                    */
      addiu $sp,$zero,0 /* $sp = cc                        */
      tux64_boot_stage0_start.rdram_initialize_chip.cc_automatic:
         /* write the current cc value in automatic mode */
         jal   tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic
         addiu $t0,$sp,0 /* branch delay slot */

         /* convert the returned cc_readback into something we can work with. */
         srl   $t0,$v0,6
         srl   $t1,$v0,13
         srl   $t2,$v0,20
         srl   $t3,$v0,4
         srl   $t4,$v0,11
         srl   $t5,$v0,18
         andi  $t0,$t0,(1 << 0)
         andi  $t1,$t1,(1 << 1)
         andi  $t2,$t2,(1 << 2)
         andi  $t3,$t3,(1 << 3)
         andi  $t4,$t4,(1 << 4)
         andi  $t5,$t5,(1 << 5)
         or    $t0,$t0,$t1
         or    $t2,$t2,$t3
         or    $t4,$t4,$t5
         or    $t0,$t0,$t2
         or    $t0,$t0,$t4

         /* compute err into $t2 */
         sltu  $t1,$t0,$a0
         beq   $t1,$zero,tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic.err_geq
         subu  $t2,$t0,$a0
         subu  $t2,$a0,$t0
         tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic.err_geq:

         /* check if we should set minerr and autocc */
         beq   $sp,$zero,tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic.advance
         sltu  $t1,$t2,$v1 /* branch delay slot */
         beq   $t1,$zero,tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic.no_advance
         nop /* branch delay slot */
         tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic.advance:
         addiu $v1,$t2,0 /* minerr = err */
         addiu $k1,$sp,0 /* autocc = cc  */
         tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic.no_advance:

         /* check if cc_readback > target_cc */
         sltu  $t1,$a0,$t0
         bne   $t1,$zero,tux64_boot_stage0_start.rdram_initialize_chip.cc_automatic.exit

         /* move to next cc value */
         addiu $sp,$sp,1 /* branch delay slot */
         addiu $t0,$t0,64
         bne   $sp,$t0,tux64_boot_stage0_start.rdram_initialize_chip.cc_automatic
         nop /* branch delay slot */
      /*tux64_boot_stage0_start.rdram_initialize_chip.cc_automatic*/
      tux64_boot_stage0_start.rdram_initialize_chip.cc_automatic.exit:

      /* make sure we chose a valid autocc value */
      beq   $k1,$zero,tux64_boot_stage0_start.rdram_initialize_chip.err_exit

      /* write our final chosen cc value */
      addiu $t0,$k1,0 /* branch delay slot */
      jal   tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic
      nop /* branch delay slot */

      /* read the DeviceType register and begin enumerating chip information */
      lw    $t0,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DEVICE_TYPE($a3)

      /* verify the chip has the expected geometry, that is we have the       */
      /* following geometry:                                                  */
      /*                                                                      */ 
      /* BankBits    -  1                                                     */
      /* RowBits     -  9                                                     */
      /* ColumnBits  - 11                                                     */
      /* Bn          -  1                                                     */
      addiu $t1,$zero,1
      addiu $t2,$zero,9
      addiu $t3,$zero,11
      srl   $t4,$t0,20
      srl   $t5,$t0,16
      srl   $t6,$t0,28
      srl   $t7,$t0,26
      andi  $t4,$t4,0x000f /* BankBits */
      bne   $t4,$t1,tux64_boot_stage0_start.rdram_initialize_chip.err_exit
      andi  $t5,$t5,0x000f /* RowBits */
      bne   $t5,$t9,tux64_boot_stage0_start.rdram_initialize_chip.err_exit
      andi  $t6,$t6,0x000f /* ColumnBits */
      bne   $t6,$t3,tux64_boot_stage0_start.rdram_initialize_chip.err_exit
      andi  $t7,$t7,0x0001 /* Bn */
      bne   $t7,$t1,tux64_boot_stage0_start.rdram_initialize_chip.err_exit
      
      /* isolate En bit from DeviceType using branch delay slot */
      srl   $t0,$t0,24 /* branch delay slot */
      andi  $t0,$t0,0x0001

      /* read the manufacturer to configure timing, we still keep $t0 holding */
      /* DeviceType so we can check more information from it. */
      sw    $t8,%lo(TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_MODE)($fp)
      lw    $t1,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DEVICE_MANUFACTURER($a3)
      sw    $t9,%lo(TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_MODE)($fp)

      /* if the manufacturer isn't NEC and the En bit isn't set, then */
      /* use slower timings.  otherwise, use default timings. */
      addiu $t2,$zero,0x0005 /* NEC's manufacturer ID */
      srl   $t1,$t1,24 /* extract least significant byte */
      lui   $t3,%hi(TUX64_BOOT_STAGE0_RDRAM_RAS_INTERVAL_DEFAULT)
      beq   $t1,$t2,tux64_boot_stage0_start.rdram_initialize_chip.default_ras_interval
      ori   $t3,$t3,%lo(TUX64_BOOT_STAGE0_RDRAM_RAS_INTERVAL_DEFAULT) /* branch delay slot */
      bne   $t0,$zero,tux64_boot_stage0_start.rdram_initialize_chip.default_ras_interval
      lui   $t4,%hi(TUX64_BOOT_STAGE0_RDRAM_RAS_INTERVAL_UNKNOWN) /* branch delay slot */
      ori   $t3,$t4,%lo(TUX64_BOOT_STAGE0_RDRAM_RAS_INTERVAL_UNKNOWN) /* branch delay slot */
      tux64_boot_stage0_start.rdram_initialize_chip.default_ras_interval:

      /* set the RasInterval using our selected timings */
      sw    $t3,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_RAS_INTERVAL($a3)

      /* settle timing circuits, also reserves $s0 for the total amount of */
      /* detected memory for the rest of stage-0. */
      lui   $t0,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_UNCACHED)
      sll   $s0,$a1,20 /* chip_id * 1MiB */
      lui   $t1,((512 * 1024) >> 16)
      addiu $t2,$zero,4
      add   $t0,$t0,$s0
      tux64_boot_stage0_start.rdram_initialize_chip.settle_timing_circuits:
         lw    $zero,0($t0)
         lw    $zero,4($t0)
         addiu $t2,$t2,-1
         bne   $t2,$zero,tux64_boot_stage0_start.rdram_initialize_chip.settle_timing_circuits
         addu  $t0,$t0,$t1 /* branch delay slot */
      /*tux64_boot_stage0_start.rdram_initialize_chip.settle_timing_circuits*/

      /* move to next chip */
      add   $a3,$a3,$a2
      b     tux64_boot_stage0_start.rdram_initialize_chip
      addiu $a1,$a1,2 /* branch delay slot */

      tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_manual:
         /* we do this to help make the code a little less confusing when     */
         /* calling from the main loop.  refer to the notes below.            */
         /*                                                                   */
         /* $t0 - current calibration value                                   */
         /* $t1 - bit flags, in little endian                                 */
         lui   $t1,(TUX64_BOOT_STAGE0_RDRAM_MODE_CURRENT_CALIBRATE_MANUAL << 8)
      tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode:
         /* this is a co-routine which writes RDRAM_REG_MODE with a current   */
         /* calibration value.  it's to be used during current calibration    */
         /* similar to how it's done in libdragon, with the exception that it */
         /* doesn't return anything.                                          */
         /*                                                                   */
         /* $t0 - current calibration value                                   */
         /* $t1 - bit flags, in little endian                                 */
         xori  $t0,$t0,0x003f
         andi  $t2,$t0,(1 << 0)
         andi  $t3,$t0,(1 << 1)
         andi  $t4,$t0,(1 << 2)
         andi  $t5,$t0,(1 << 3)
         andi  $t6,$t0,(1 << 4)
         andi  $t7,$t0,(1 << 5)
         sll   $t2,$t2,6
         sll   $t3,$t3,13
         sll   $t4,$t4,20
         sll   $t5,$t5,4
         sll   $t6,$t6,11
         sll   $t7,$t7,18
         or    $t1,$t1,$t2
         or    $t3,$t3,$t4
         or    $t5,$t5,$t6
         or    $t1,$t1,$t7
         or    $t3,$t3,$t5
         or    $t1,$t1,$t3
         jr    $ra
         sw    $t1,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_MODE($a3) /* branch delay slot */
      /*tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode*/

      tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic:
         /* this is an extension of the below co-routine which loads $t1 with */
         /* the little-endian bitflags for automatic current calibration.     */
         /* we then also need to read and return the cc value.  note that we  */
         /* assume $at isn't clobbered by the below function calls.           */
         /*                                                                   */
         /* $t0 - current calibration value                                   */
         /* $v0 - (return value) read current calibration value from chip,    */
         /*       before any endian or bit conversions are performed.         */
         addiu $at,$ra,0
         jal   tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode
         lui   $t1,(TUX64_BOOT_STAGE0_RDRAM_MODE_CURRENT_CALIBRATE_AUTOMATIC << 8) /* branch delay slot */

         /* read the current calibration value */
         jal   tux64_boot_stage0_rdram_wait
         nop /* branch delay slot */
         lw    $zero,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_MODE($a3)
         jr    $at
         lw    $v0,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_MODE($a3) /* branch delay slot */
      /*tux64_boot_stage0_start.rdram_initialize_chip.write_reg_mode_automatic*/
      
   /*tux64_boot_stage0_start.rdram_initialize_chip*/

   /* if we encountered an error during initialization of the last chip, make */
   /* sure to invalidate the chip ID. */
   tux64_boot_stage0_start.rdram_initialize_chip.err_exit:
   lui   $t0,%hi(TUX64_BOOT_STAGE0_RDRAM_DEVICEID_INVALID_ENCODED)
   ori   $t0,$t0,%lo(TUX64_BOOT_STAGE0_RDRAM_DEVICEID_INVALID_ENCODED)
   sw    $t0,TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DEVICE_ID($a3)
   tux64_boot_stage0_start.rdram_initialize_chip.exit:

   /* configure RI refresh rate and skip past memory detection */
   addiu $t1,$a1,-1
   lui   $t0,%hi(TUX64_BOOT_STAGE0_RI_REFRESH_BASE)
   andi  $t1,$t1,0x000f
   ori   $t0,$t0,%lo(TUX64_BOOT_STAGE0_RI_REFRESH_BASE)
   sll   $t1,$t1,19
   or    $t0,$t0,$t1
   sw    $t0,%lo(TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_REFRESH)($s1)
   b     tux64_boot_stage0_start.skip_detect_total_memory
   lw    $zero,%lo(TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_REFRESH)($s1) /* branch delay slot */

   tux64_boot_stage0_start.skip_rdram_initialization:

   /* detect the total amount of available memory.  this is done by probing   */
   /* memory 1MiB at a time.  this number is chosen because the smallest size */
   /* of RDRAM chips is 1MiB (despite never being used in the N64), so that's */
   /* the smallest reasonable size to probe.  for the ique player, we always  */
   /* have 8MiB of available memory (technically 16MiB, but the RI only       */
   /* exposes 8MiB), so this code still works.  we cap the upper limit to     */
   /* 8MiB since that's the maximum amount of memory possible.                */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_DETECT_TOTAL_MEMORY

   lui   $a0,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_UNCACHED)
   addiu $s0,$zero,0
   lui   $s1,TUX64_BOOT_STAGE0_1MIB_HI
   lui   $a2,(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_UNCACHED >> 16) + (TUX64_BOOT_STAGE0_1MIB_HI * 8)
   tux64_boot_stage0_start.detect_total_memory:
      ld    $t3,0($a0)     /* load current value            */
      nor   $t4,$t3,$zero  /* flips all bits                */
      sd    $t4,0($a0)     /* store the flipped bits        */
      ld    $t5,0($a0)     /* reload to see what was stored */
      sd    $t3,0($a0)     /* restore the old value         */

      /* if the values differ, then either the memory chip is bad or we're at */
      /* the end of available memory.  exit the loop. */
      bne   $t4,$t5,tux64_boot_stage0_start.detect_total_memory.exit

      /* advance the memory pointer using the branch delay slot */
      addu  $a0,$a0,$s1

      /* the maximum amount of possible memory is 8MiB.  if we're not past */
      /* 8MiB of memory, continue looping */
      bne   $a0,$a2,tux64_boot_stage0_start.detect_total_memory

      /* increment the total detected memory using the branch delay slot */
      addu  $s0,$s0,$s1
   /*tux64_boot_stage0_start.detect_total_memory*/

   tux64_boot_stage0_start.detect_total_memory.exit:
   tux64_boot_stage0_start.skip_detect_total_memory:

   /* from this point forward, $s0 is reserved for the total system memory, */
   /* in bytes until stage-1 begins.  this will be passed as an argument to */
   /* stage-1 to avoid using memory unnecessarily. */

   /* load the boot header into RDRAM via PI DMA */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_BOOT_HEADER

   /* reserve $a0 for the PI base address and $a1 for the high-order */
   /* cartridge ROM offset */
   lui   $a0,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PI)
   lui   $a1,%hi(TUX64_PLATFORM_MIPS_N64_PI_BUS_DOMAIN_OFFSET_DOM1_ROM)

   /* halt any active PI DMA operations.  this needs to be done because we */
   /* could be coming from a reset in the middle if a PI DMA operation. */
   addiu $t2,$zero,TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY
   sw    $t2,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_STATUS)($a0)

   /* initiate the transfer in the background, we don't need to invalidate */
   /* cache as we're initializing it below anyways.  also note we don't need */
   /* to wait on any previous PI DMA operations since we halted them above */
   addiu $t3,$zero,%lo(tux64_boot_stage1_boot_header)
   addiu $t4,$zero,(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES - 1)
   ori   $t5,$a1,TUX64_BOOT_STAGE0_BOOT_HEADER_ADDRESS_CARTRIDGE_ROM_LO
   sw    $t3,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_DRAM_ADDR)($a0)
   sw    $t5,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_CART_ADDR)($a0)
   sw    $t4,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_WR_LEN)($a0)

   /* initialize the CPU caches, setting each line to 'invalid', continue */
   /* loading the boot header in the background */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_CPU_CACHE_INITIALIZE
   lui   $t2,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED)
   lui   $t3,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED)
   lui   $a2,%hi(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED)
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_TAGLO /* write 'invalid' tag */
   mtc0  $zero,TUX64_BOOT_STAGE0_COP0_REGISTER_TAGHI /* must be zero, undefined on boot */
   addiu $t2,$t2,(TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_INSTRUCTION * TUX64_PLATFORM_MIPS_VR4300_CACHE_LINE_COUNT_INSTRUCTION)
   addiu $t3,$t3,(TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_DATA        * TUX64_PLATFORM_MIPS_VR4300_CACHE_LINE_COUNT_DATA)
   tux64_boot_stage0_start.initialize_cache:
      addiu $t2,$t2,-TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_INSTRUCTION
      addiu $t3,$t3,-TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_DATA
      cache (TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_INSTRUCTION | (TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_STORE_TAG << 2)),0($t2)
      bne   $a2,$t2,tux64_boot_stage0_start.initialize_cache
      cache (TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA | (TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_STORE_TAG << 2)),0($t3)
   /*tux64_boot_stage0_start.initialize_cache*/

   /* we will now reserve $a2 for the cached RDRAM base address. here are the */
   /* current reserved registers:                                             */
   /* $a0       - PI MMIO registers base address                              */
   /* $a1       - cartridge ROM base address (physical address)               */
   /* $a2       - cached RDRAM base address                                   */
   /* $s0       - total memory                                                */
   /* $s2       - ique boolean                                                */
   /* $s3-$s7   - IPL2 arguments                                              */

   /* calculate the checksum of the boot header and verify it matches what's */
   /* given by the header */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_BOOT_HEADER

   /* wait for the boot header to have finished loading */
   tux64_boot_stage0_start.pi_io_dma_spinlock.boot_header:
      lw    $at,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_STATUS)($a0)
      andi  $at,$at,TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY
      bne   $at,$zero,tux64_boot_stage0_start.pi_io_dma_spinlock.boot_header
   /*tux64_boot_stage0_start.pi_io_dma_spinlock.boot_header*/

   /* verify the boot header magic is present */
   lui   $t0,%hi(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_MAGIC) /* branch delay slot */
   lw    $t1,%lo(tux64_boot_stage1_boot_header)+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_MAGIC($a2)
   ori   $t0,$t0,%lo(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_MAGIC)
   bne   $t0,$t1,tux64_boot_stage0_halt
   
   /* calculate the header's checksum and verify it, this also gets the boot */
   /* header into cache so that rambus goes vroom vroom, also the next */
   /* instruction executes in the above branch delay slot */
   lw    $t2,%lo(tux64_boot_stage1_boot_header)+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_CHECKSUM($a2)
   addiu $t0,$a2,%lo(tux64_boot_stage1_boot_header)+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA
   jal   tux64_boot_stage0_checksum_calculate_and_verify
   addiu $t1,$a2,%lo(tux64_boot_stage1_boot_header)+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES

   /* begin loading the stage-1 binary into memory */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_LOAD_STAGE1

   /* read the stage-1 required memory and length */
   lw    $k0,%lo(tux64_boot_stage1_boot_header)+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_MEMORY($a2)
   lw    $k1,%lo(tux64_boot_stage1_boot_header)+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_LENGTH($a2)

   /* check if we have enough memory to load the stage-1 binary. */
   addiu $t0,$k0,-TUX64_BOOT_STAGE1_MEMORY_RDRAM_RESERVED
   sltu  $t0,$s0,$t0
   bne   $t0,$zero,tux64_boot_stage0_halt

   /* copy the stage-1 bootloader into memory via PI DMA, no need to */
   /* invalidate cache since we only have the header in data cache, and since */
   /* a data cache line is 16 bytes, we will never have the stage-1 payload */
   /* on the same cache line as the header.  note that this concerns data */
   /* cache.  if we were in instruction cache, we would need to align to a */
   /* 32-byte boundary. */
   addiu $t1,$zero,TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_RDRAM_LO /* branch delay slot */
   ori   $t2,$a1,TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_CARTRIDGE_ROM_LO
   addiu $t3,$k1,-1
   sw    $t1,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_DRAM_ADDR)($a0)
   sw    $t2,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_CART_ADDR)($a0)
   sw    $t3,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_WR_LEN)($a0)

   /* calculate the total available memory for stage-1, reserving $s1 */
   subu  $s1,$s0,$k0

   /* load the boot flags and stage-1 checksum, reserving $fp */
   lw    $fp,%lo(tux64_boot_stage1_boot_header)+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FLAGS($a2)

   /* as a reminder, here are the current registers we care about:            */
   /* $a0       - PI MMIO registers base address                              */
   /* $a1       - cartridge ROM base address (physical address)               */
   /* $a2       - cached RDRAM base address                                   */
   /* $s0       - total memory                                                */
   /* $s1       - available memory                                            */
   /* $s2       - ique boolean                                                */
   /* $s3-$s7   - IPL2 arguments                                              */
   /* $k1       - stage-1 length                                              */
   /* $fp       - boot flags                                                  */

   /* calculate the checksum of the stage-1 binary and verify it's correct */
   /* unless the NO_CHECKSUM flag is set */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_CHECK_STAGE1

   /* check if the NO_CHECKSUM flag is set, freeing $fp from this point */
   /* forward */
   andi  $fp,$fp,TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_NO_CHECKSUM
   bne   $fp,$zero,tux64_boot_stage0_start.skip_checksum_stage1

   /* wait for the PI DMA operation to complete, then verify the checksum */
   /* branch delay slot.  we now free $k1. */
   addiu $t0,$a2,TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_RDRAM_LO /* branch delay slot */

   tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_checksum:
      lw    $at,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_STATUS)($a0)
      andi  $at,$at,TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY
      bne   $at,$zero,tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_checksum
   /*tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_checksum*/

   addu  $t1,$t0,$k1 /* branch delay slot */
   jal   tux64_boot_stage0_checksum_calculate_and_verify
   lw    $t2,%lo(tux64_boot_stage1_boot_header)+TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_CHECKSUM($a2) /* branch delay slot */

   tux64_boot_stage0_start.skip_checksum_stage1:

   /* this is now our current register allocation:                            */
   /* $a0       - PI MMIO registers base address                              */
   /* $a1       - cartridge ROM base address (physical address)               */
   /* $a2       - cached RDRAM base address                                   */
   /* $s0       - total memory                                                */
   /* $s1       - available memory                                            */
   /* $s2       - ique boolean                                                */
   /* $s3-$s7   - IPL2 arguments                                              */
   /* $fp       - boot flags                                                  */

   /* calculate the stage-1 start address and prepare to start stage-1 */
   jal   tux64_boot_stage0_status_code_write
   addiu $t0,$zero,TUX64_BOOT_STAGE0_STATUS_CODE_START_STAGE1
   
   /* wait for PI DMA, as we could reach here if we skipped the stage-1 */
   /* checksum without waiting on PI DMA to complete. */
   tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_start:
      lw    $at,%lo(TUX64_PLATFORM_MIPS_N64_PI_ADDRESS_STATUS)($a0)
      andi  $at,$at,TUX64_BOOT_STAGE0_PI_STATUS_BIT_MASK_DMA_IO_BUSY
      bne   $at,$zero,tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_start
   /*tux64_boot_stage0_start.pi_io_dma_spinlock.payload_stage1_start*/

   /* initialize the stack and start stage-1 */
   addiu $k0,$a2,TUX64_BOOT_STAGE0_PAYLOAD_STAGE1_ADDRESS_RDRAM_LO /* branch delay slot */
   jr    $k0
   addiu $sp,$a2,TUX64_BOOT_STAGE1_STACK_BYTES
/*tux64_boot_stage0_start*/

   /* TODO: add multiple of these, one for each CIC revision.  only do this */
   /* when this code is finalized, as it takes long enough just to find one. */
   .section .cic
tux64_boot_stage0_cic:
   .word 0x00000d3a
   .word 0x1417fa12
/*tux64_boot_stage0_cic*/

