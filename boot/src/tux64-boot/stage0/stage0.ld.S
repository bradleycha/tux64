/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage0/stage0.ld.S - Stage-0 bootloader linker script. */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include "tux64-boot/layout.h"

ENTRY(tux64_boot_stage0_start)

/* a neat trick we have is the CIC data can be overwritten by the boot status */
/* code, which means we aren't wasting any memory at runtime by including the */
/* CIC data. */
TUX64_BOOT_STAGE0_MEMORY_ADDRESS_CIC_DATA
 = TUX64_BOOT_LAYOUT_STATUS_ADDRESS;

PHDRS
{
   image PT_LOAD AT(TUX64_BOOT_LAYOUT_STAGE0_LOAD_ADDRESS) FLAGS(7);
}

SECTIONS
{
   /* the stage-0 code binary to load in place of the IPL3 */
   .rsp_dmem (TUX64_BOOT_LAYOUT_STAGE0_LOAD_ADDRESS) : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } :image =0x00

   /* store the .bss items at the start of RSP IMEM */
   .rsp_imem : {
      *(.bss*);
   } :noload_rsp_imem =0x00

   /* store CIC data in seperate section to be extracted as binary data */
   .cic (TUX64_BOOT_STAGE0_MEMORY_ADDRESS_CIC_DATA) : {
      KEEP(*(.cic));
   } :noload_rsp_dmem =0x00
}

