/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.ld.S - Stage-2 bootloader linker script. */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"
#include "tux64-boot/stage2/stack.ld.h"
#include "tux64-boot/layout.h"

ENTRY(tux64_boot_stage2_start)

PHDRS
{
   image PT_LOAD AT(TUX64_BOOT_LAYOUT_STAGE2_LOAD_ADDRESS) FLAGS(7);
}

SECTIONS
{
   .rdram (TUX64_BOOT_LAYOUT_STAGE2_LOAD_ADDRESS) : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);

      /* same trick with $gp from stage-1 */
      _gp = ALIGN(8) + 0x8000;

      *(.sdata*);
   } :image =0x00

   .rdram.bss (ADDR(.rdram) + SIZEOF(.rdram)) : {
      *(.sbss*);
      *(.bss*);
   } :noload_rdram_cached =0x00
}

