/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/header.ld.h - Boot header linker script definitions.   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_HEADER_LD_H
#define _TUX64_BOOT_HEADER_LD_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"
#include "tux64-boot/layout.h"

SECTIONS
{
   .rdram.boot_header (TUX64_BOOT_LAYOUT_HEADER_ADDRESS) : {
      tux64_boot_header = .;

      . += TUX64_BOOT_LAYOUT_HEADER_BYTES;
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_HEADER_LD_H */

