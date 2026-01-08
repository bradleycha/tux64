/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/status.c - Boot status code implementation.            */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/status.h"

#include <tux64/endian.h>

#define TUX64_BOOT_STATUS_MAGIC_HI \
   (0x53544147u) /* STAG */
#define TUX64_BOOT_STATUS_MAGIC_LO \
   (0x45003a00u) /* Ex:x */

struct Tux64BootStatusCode {
   Tux64UInt32 hi;
   Tux64UInt32 lo;
};

static volatile struct Tux64BootStatusCode
tux64_boot_status_code
__attribute__((section(".tux64_boot.status")));

void
tux64_boot_status_code_initialize(void) {
   if (!TUX64_BOOT_CONFIG_STATUS) {
      return;
   }

   tux64_boot_status_code.hi = tux64_endian_convert_uint32(
      TUX64_LITERAL_UINT32(TUX64_BOOT_STATUS_MAGIC_HI),
      TUX64_ENDIAN_FORMAT_BIG
   );

   return;
}

void
tux64_boot_status_code_write(
   char stage,
   char code
) {
   Tux64UInt32 lo;

   if (!TUX64_BOOT_CONFIG_STATUS) {
      return;
   }

   lo = TUX64_LITERAL_UINT32(TUX64_BOOT_STATUS_MAGIC_LO);
   lo = lo | ((Tux64UInt32)stage << TUX64_LITERAL_UINT8(16u));
   lo = lo | ((Tux64UInt32)code  << TUX64_LITERAL_UINT8( 0u));

   tux64_boot_status_code.lo = tux64_endian_convert_uint32(
      lo, TUX64_ENDIAN_FORMAT_BIG
   );

   return;
}

