/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/memory.c - Implementations for memory           */
/*    management.                                                             */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/memory.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include "tux64-boot/arena-allocator.h"

struct Tux64BootStage1MemoryStatistics {
   Tux64UInt32 total;
   Tux64UInt32 free;
   struct Tux64BootArenaAllocator heap;
};

static struct Tux64BootStage1MemoryStatistics
tux64_boot_stage1_memory_statistics;

/* used to allow the linker to determine the initial start of free memory. */
/* this does lead to redundant data, where we both calculate free memory at */
/* runtime and the start of free memory at compile-time, but it saves a few */
/* instructions so what-evs. */
TUX64_LINKER_SYMBOL(tux64_boot_stage1_memory_heap_start);

void
tux64_boot_stage1_memory_initialize(
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_free
) {
   Tux64UIntPtr heap_start;
   Tux64UIntPtr heap_end;

   tux64_boot_stage1_memory_statistics.total = memory_total;
   tux64_boot_stage1_memory_statistics.free = memory_free;

   heap_start  = (Tux64UIntPtr)tux64_boot_stage1_memory_heap_start;
   heap_end    = TUX64_LITERAL_UINTPTR(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED) + memory_total;

   tux64_boot_arena_allocator_initialize(
      &tux64_boot_stage1_memory_statistics.heap,
      (void *)heap_start,
      (void *)heap_end
   );

   return;
}

Tux64UInt32
tux64_boot_stage1_memory_total(void) {
   return tux64_boot_stage1_memory_statistics.total;
}

Tux64UInt32
tux64_boot_stage1_memory_free(void) {
   return tux64_boot_stage1_memory_statistics.free;
}

Tux64Boolean
tux64_boot_stage1_memory_alloc_inplace(
   void * address,
   Tux64UInt32 bytes
) {
   Tux64Boolean retn;

   retn = tux64_boot_arena_allocator_alloc_inplace(
      &tux64_boot_stage1_memory_statistics.heap,
      address,
      bytes
   );

   if (retn == TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_memory_statistics.free -= bytes;
   }

   return retn;
}

void *
tux64_boot_stage1_memory_alloc(
   Tux64UInt32 bytes,
   Tux64UInt8 alignment
) {
   void * retn;

   retn = tux64_boot_arena_allocator_alloc(
      &tux64_boot_stage1_memory_statistics.heap,
      bytes,
      alignment
   );

   if (retn != TUX64_NULLPTR) {
      tux64_boot_stage1_memory_statistics.free -= bytes;
   }

   return retn;
}

