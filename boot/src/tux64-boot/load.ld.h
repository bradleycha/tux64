/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/load.ld.h - Linker script definitions for load         */
/*    allocations struct.                                                     */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_LOAD_LD_H
#define _TUX64_BOOT_LOAD_LD_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"
#include "tux64-boot/layout.h"

SECTIONS
{
   .rdram.load_allocations (TUX64_BOOT_LAYOUT_LOAD_ALLOCATIONS_ADDRESS) : {
      tux64_boot_load_allocations = .;

      . += TUX64_BOOT_LAYOUT_LOAD_ALLOCATIONS_BYTES;
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_LOAD_LD_H */

