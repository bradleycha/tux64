/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.ld.S - Stage-2 bootloader linker script. */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"

#include "tux64-boot/stage2/stack.ld.h"

ENTRY(tux64_boot_stage2_start)

SECTIONS
{
   /* place the stage-2 code and data into RSP IMEM */
   .rsp_imem : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);

      /* same trick with $gp from stage-1 */
      _gp = ALIGN(8) + 0x8000;

      *(.sdata*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_IMEM =0x00

   /* place the stage-2 uninitialized variables into RSP DMEM */
   .rsp_dmem.bss : {
      /* same trick with $gp from stage-1 */
      *(.sbss*);

      *(.bss*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

