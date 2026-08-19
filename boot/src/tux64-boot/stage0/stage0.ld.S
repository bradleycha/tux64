/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage0/stage0.ld.S - Stage-0 bootloader linker script. */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include "tux64-boot/status.h"
#include "tux64-boot/stage1/stack.ld.h"

ENTRY(tux64_boot_stage0_start)

/* a neat trick we have is the CIC data can be overwritten by the boot status */
/* code, which means we aren't wasting any memory at runtime by including the */
/* CIC data. */
TUX64_BOOT_STAGE0_MEMORY_ADDRESS_CIC_DATA
 = TUX64_BOOT_STATUS_ADDRESS;

SECTIONS
{
   /* the stage-0 code binary to load in place of the IPL3 */
   .rsp_dmem (TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM + 0x40) : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00

   /* store the .bss items at the start of RSP IMEM */
   .rsp_imem : {
      *(.bss*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_IMEM =0x00

   /* store CIC data in seperate section to be extracted as binary data */
   .cic (TUX64_BOOT_STAGE0_MEMORY_ADDRESS_CIC_DATA) : {
      KEEP(*(.cic));
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

