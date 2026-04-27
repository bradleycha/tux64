/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/memory.c - Implementations for memory           */
/*    management.                                                             */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/memory.h"

struct Tux64BootStage1MemoryStatistics {
   Tux64UInt32 total;
   Tux64UInt32 free;
   void * top;
};

static struct Tux64BootStage1MemoryStatistics
tux64_boot_stage1_memory_statistics;

/* used to allow the linker to determine the initial start of free memory. */
/* this does lead to redundant data, where we both calculate free memory at */
/* runtime and the start of free memory at compile-time, but it saves a few */
/* instructions so what-evs. */
extern Tux64UInt8 tux64_boot_stage1_memory_top_initial [];

void
tux64_boot_stage1_memory_initialize(
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_free
) {
   tux64_boot_stage1_memory_statistics.total = memory_total;
   tux64_boot_stage1_memory_statistics.free = memory_free;
   tux64_boot_stage1_memory_statistics.top = &tux64_boot_stage1_memory_top_initial;
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

void *
tux64_boot_stage1_memory_top(void) {
   return tux64_boot_stage1_memory_statistics.top;
}

Tux64Boolean
tux64_boot_stage1_memory_region_is_free(
   const void * address,
   Tux64UInt32 bytes
) {
   if (address < tux64_boot_stage1_memory_top()) {
      return TUX64_BOOLEAN_FALSE;
   }
   if (bytes > tux64_boot_stage1_memory_free()) {
      return TUX64_BOOLEAN_FALSE;
   }

   return TUX64_BOOLEAN_TRUE;
}

