/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/flag.c - Implementations for checking boot flags.      */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/flag.h"

#include "tux64-boot/header.h"

Tux64Boolean
tux64_boot_flag_checksum(void) {
   if (!TUX64_BOOT_CONFIG_CHECKSUM) {
      return TUX64_BOOLEAN_FALSE;
   }

   return (tux64_boot_header_flag_no_checksum() == TUX64_BOOLEAN_FALSE);
}

Tux64Boolean
tux64_boot_flag_delay(void) {
   if (!TUX64_BOOT_CONFIG_DELAY) {
      return TUX64_BOOLEAN_FALSE;
   }

   return (tux64_boot_header_flag_no_delay() == TUX64_BOOLEAN_FALSE);
}

Tux64Boolean
tux64_boot_flag_memory_display(void) {
   if (!TUX64_BOOT_CONFIG_MEMORY_DISPLAY) {
      return TUX64_BOOLEAN_FALSE;
   }

   return (tux64_boot_header_flag_memory_display() == TUX64_BOOLEAN_TRUE);
}
