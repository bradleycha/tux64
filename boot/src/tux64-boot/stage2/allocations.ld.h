/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/allocations.ld.h - Linker script definitions    */
/*    for stage-2 load allocations struct.                                    */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE2_ALLOCATIONS_LD_H
#define _TUX64_BOOT_STAGE2_ALLOCATIONS_LD_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"
#include "tux64-boot/load.h"
#include "tux64-boot/stage2/allocations.h"

SECTIONS
{
   .rsp_dmem.allocations (TUX64_BOOT_STAGE2_ALLOCATIONS_ADDRESS) : {
      tux64_boot_stage2_allocations = .;

      . += TUX64_BOOT_LOAD_ALLOCATIONS_BYTES;
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_ALLOCATIONS_LD_H */

