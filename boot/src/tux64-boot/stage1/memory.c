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
#include "tux64-boot/stage1/arena-allocator.h"
#include "tux64-boot/stage2/memory.h"

/* we two seperate heaps so we can perform memory allocations for stage-2     */
/* ahead of time without including the entire arena allocator in the stage-2  */
/* binary.  this also lets us better report allocation errors when we have    */
/* the fbcon loaded.                                                          */
struct Tux64BootStage1MemoryStatisticsHeap {
   struct Tux64BootStage1ArenaAllocator stage1;
   struct Tux64BootStage1ArenaAllocator stage2;
};

struct Tux64BootStage1MemoryStatistics {
   Tux64UInt32 total;
   Tux64UInt32 free;
   struct Tux64BootStage1MemoryStatisticsHeap heap;
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
   Tux64UInt32 heap_stage1_start;
   Tux64UInt32 heap_stage1_end;
   Tux64UInt32 heap_stage2_start;
   Tux64UInt32 heap_stage2_end;

   tux64_boot_stage1_memory_statistics.total = memory_total;
   tux64_boot_stage1_memory_statistics.free = memory_free;

   heap_stage1_start = (Tux64UIntPtr)(Tux64UIntPtr)tux64_boot_stage1_memory_heap_start;
   heap_stage1_end   = TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED) + memory_total;

   heap_stage2_start = TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED + TUX64_BOOT_STAGE2_MEMORY_RDRAM_RESERVED);
   heap_stage2_end   = TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED) + memory_total;

   tux64_boot_stage1_arena_allocator_initialize(
      &tux64_boot_stage1_memory_statistics.heap.stage1,
      heap_stage1_start,
      heap_stage1_end
   );
   tux64_boot_stage1_arena_allocator_initialize(
      &tux64_boot_stage1_memory_statistics.heap.stage2,
      heap_stage2_start,
      heap_stage2_end
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
tux64_boot_stage1_memory_stage1_alloc_inplace(
   Tux64UInt32 address,
   Tux64UInt32 bytes
) {
   Tux64Boolean retn;

   retn = tux64_boot_stage1_arena_allocator_alloc_inplace(
      &tux64_boot_stage1_memory_statistics.heap.stage1,
      address,
      bytes
   );

   if (retn == TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_memory_statistics.free -= bytes;
   }

   return retn;
}

Tux64UInt32
tux64_boot_stage1_memory_stage1_alloc(
   Tux64UInt32 bytes,
   Tux64UInt8 alignment
) {
   Tux64UInt32 retn;

   retn = tux64_boot_stage1_arena_allocator_alloc(
      &tux64_boot_stage1_memory_statistics.heap.stage1,
      bytes,
      alignment
   );

   if (retn != TUX64_LITERAL_UINT32(0u)) {
      tux64_boot_stage1_memory_statistics.free -= bytes;
   }

   return retn;
}

Tux64Boolean
tux64_boot_stage1_memory_stage2_alloc_inplace(
   Tux64UInt32 address,
   Tux64UInt32 bytes
) {
   return tux64_boot_stage1_arena_allocator_alloc_inplace(
      &tux64_boot_stage1_memory_statistics.heap.stage2,
      address,
      bytes
   );
}

Tux64UInt32
tux64_boot_stage1_memory_stage2_alloc(
   Tux64UInt32 bytes,
   Tux64UInt8 alignment
) {
   return tux64_boot_stage1_arena_allocator_alloc(
      &tux64_boot_stage1_memory_statistics.heap.stage2,
      bytes,
      alignment
   );
}

