/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/checksum.c - Implementations for specialized fetcher   */
/*    64/32 checksum algorithm.                                               */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/checksum.h"

#include <tux64/endian.h>

void
tux64_boot_checksum_fletcher_64_32_initialize(
   struct Tux64BootChecksumFletcher6432Context * context
) {
   context->sum_hi = TUX64_LITERAL_UINT32(0u);
   context->sum_lo = TUX64_LITERAL_UINT32(0u);
   return;
}

void
tux64_boot_checksum_fletcher_64_32_digest(
   struct Tux64BootChecksumFletcher6432Context * context,
   const Tux64UInt8 * data,
   Tux64UInt32 bytes
) {
   const Tux64UInt32 * iter_data;
   Tux64UInt32 word;

   iter_data = (const Tux64UInt32 *)data;

   do {
      word = tux64_endian_convert_uint32(*iter_data++, TUX64_ENDIAN_FORMAT_BIG);
      bytes -= TUX64_LITERAL_UINT32(sizeof(*iter_data));

      context->sum_hi += word;
      context->sum_lo += context->sum_hi;
   } while (bytes != TUX64_LITERAL_UINT32(0u));

   return;
}

Tux64UInt32
tux64_boot_checksum_fletcher_64_32_finalize(
   struct Tux64BootChecksumFletcher6432Context * context
) {
   return context->sum_lo - context->sum_hi;
}

