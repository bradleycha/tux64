/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/cache.c - Implementations for cache management.        */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/cache.h"

#include <tux64/platform/mips/vr4300/cache.h>

typedef void (*Tux64BootCacheOperation)(
   const void * address
);

static void
tux64_boot_cache_operation(
   const void * address,
   Tux64UInt32 bytes,
   Tux64UInt32 bytes_per_line,
   Tux64BootCacheOperation cache_operation
) {
   const Tux64UInt8 * iter;

   iter = (const Tux64UInt8 *)address;

   do {
      cache_operation(iter);
      iter  += bytes_per_line;
      bytes -= bytes_per_line;
   } while (bytes >= bytes_per_line);

   return;
}

void
tux64_boot_cache_invalidate_instruction(
   const void * address,
   Tux64UInt32 bytes
) {
   tux64_boot_cache_operation(
      address,
      bytes,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_INSTRUCTION),
      tux64_platform_mips_vr4300_cache_operation_instruction_hit_invalidate
   );
   return;
}

void
tux64_boot_cache_invalidate_data(
   const void * address,
   Tux64UInt32 bytes
) {
   tux64_boot_cache_operation(
      address,
      bytes,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_DATA),
      tux64_platform_mips_vr4300_cache_operation_data_hit_invalidate
   );
   return;
}

void
tux64_boot_cache_invalidate(
   const void * address,
   Tux64UInt32 bytes
) {
   tux64_boot_cache_invalidate_instruction(address, bytes);
   tux64_boot_cache_invalidate_data(address, bytes);
   return;
}

void
tux64_boot_cache_flush_data(
   const void * address,
   Tux64UInt32 bytes
) {
   tux64_boot_cache_operation(
      address,
      bytes,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_DATA),
      tux64_platform_mips_vr4300_cache_operation_data_hit_write_back
   );
   return;
}

