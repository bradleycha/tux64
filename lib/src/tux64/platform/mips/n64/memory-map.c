/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/memory-map.c - Implementations for memory  */
/*    map helper functions.                                                   */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/n64/memory-map.h"

const void *
tux64_platform_mips_n64_memory_map_physical_to_direct_cached(
   const void * address
) {
   Tux64UIntPtr retn;

   retn = (Tux64UIntPtr)address;
   retn = retn + TUX64_LITERAL_UINTPTR(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_CACHED);

   return (const void *)retn;
}

const void *
tux64_platform_mips_n64_memory_map_physical_to_direct_uncached(
   const void * address
) {
   Tux64UIntPtr retn;

   retn = (Tux64UIntPtr)address;
   retn = retn + TUX64_LITERAL_UINTPTR(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED);

   return (const void *)retn;
}

const void *
tux64_platform_mips_n64_memory_map_direct_cached_to_physical(
   const void * address
) {
   Tux64UIntPtr retn;

   retn = (Tux64UIntPtr)address;
   retn = retn - TUX64_LITERAL_UINTPTR(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_CACHED);

   return (const void *)retn;
}

const void *
tux64_platform_mips_n64_memory_map_direct_uncached_to_physical(
   const void * address
) {
   Tux64UIntPtr retn;

   retn = (Tux64UIntPtr)address;
   retn = retn - TUX64_LITERAL_UINTPTR(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED);

   return (const void *)retn;
}

const void *
tux64_platform_mips_n64_memory_map_direct_cached_to_direct_uncached(
   const void * address
) {
   const void * retn;

   retn = address;
   retn = tux64_platform_mips_n64_memory_map_direct_cached_to_physical(retn);
   retn = tux64_platform_mips_n64_memory_map_physical_to_direct_uncached(retn);

   return retn;
}

const void *
tux64_platform_mips_n64_memory_map_direct_uncached_to_direct_cached(
   const void * address
) {
   const void * retn;

   retn = address;
   retn = tux64_platform_mips_n64_memory_map_direct_uncached_to_physical(retn);
   retn = tux64_platform_mips_n64_memory_map_physical_to_direct_cached(retn);

   return retn;
}

