/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/arena-allocator.c - Implementations for an      */
/*    arena allocator.                                                        */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/arena-allocator.h"

#include <tux64/math.h>
#include "tux64-boot/halt.h"

/*----------------------------------------------------------------------------*/
/* the main benefit to our allocator is it doesn't support freeing blocks.    */
/* this allows us to simplify our code because each block is stored           */
/* sequentially, so we can eliminate some code required to support random     */
/* allocation.  i.e. we implement a push-only stack for our allocation list.  */
/*                                                                            */
/* We also assume we never allocate more than the maximum number of           */
/* allocation blocks.  This eliminates some debug checks, at the cost of      */
/* safety.                                                                    */
/*----------------------------------------------------------------------------*/

void
tux64_boot_stage1_arena_allocator_initialize(
   struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 address_space_start,
   Tux64UInt32 address_space_end
) {
   allocator->address_space_start   = address_space_start;
   allocator->address_space_end     = address_space_end;
   allocator->blocks_allocated      = TUX64_LITERAL_UINT8(0u);
   return;
}

static struct Tux64BootStage1ArenaAllocatorBlock *
tux64_boot_stage1_arena_allocator_push_block(
   struct Tux64BootStage1ArenaAllocator * allocator
) {
   struct Tux64BootStage1ArenaAllocatorBlock * block;

   if (
      TUX64_BOOT_CONFIG_DEBUG &&
      allocator->blocks_allocated == TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_ARENA_ALLOCATOR_MAX_ALLOCATIONS)
   ) {
      /* it's easy to accidentally allocate too much, so i think it's a good */
      /* idea to add a debug-only safety check here. */
      tux64_boot_halt();
      TUX64_UNREACHABLE;
   }

   block = &allocator->blocks[allocator->blocks_allocated];
   allocator->blocks_allocated++;

   return block;
}

static Tux64Boolean
tux64_boot_stage1_arena_allocator_region_is_within_address_space(
   const struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 ptr_start,
   Tux64UInt32 ptr_end
) {
   Tux64UInt32 address_space_start;
   Tux64UInt32 address_space_end;

   address_space_start  = allocator->address_space_start;
   address_space_end    = allocator->address_space_end;

   if (ptr_start < address_space_start) {
      return TUX64_BOOLEAN_FALSE;
   }
   if (ptr_end > address_space_end) {
      return TUX64_BOOLEAN_FALSE;
   }

   return TUX64_BOOLEAN_TRUE;
}

static Tux64Boolean
tux64_boot_stage1_arena_allocator_regions_overlap(
   Tux64UInt32 lhs_start,
   Tux64UInt32 lhs_end,
   Tux64UInt32 rhs_start,
   Tux64UInt32 rhs_end
) {
   if (rhs_start < lhs_end) {
      return TUX64_BOOLEAN_FALSE;
   }

   (void)lhs_start;
   (void)rhs_end;
   return TUX64_BOOLEAN_TRUE;
}

static Tux64Boolean
tux64_boot_stage1_arena_allocator_region_excludes_block(
   const struct Tux64BootStage1ArenaAllocatorBlock * block,
   Tux64UInt32 ptr_start,
   Tux64UInt32 ptr_end
) {
   Tux64UInt32 block_start;
   Tux64UInt32 block_end;

   block_start = block->start;
   block_end   = block->end;

   if (ptr_start < block_start) {
      return tux64_boot_stage1_arena_allocator_regions_overlap(
         ptr_start,
         ptr_end,
         block_start,
         block_end
      );
   }

   return tux64_boot_stage1_arena_allocator_regions_overlap(
      block_start,
      block_end,
      ptr_start,
      ptr_end
   );
}

static Tux64Boolean
tux64_boot_stage1_arena_allocator_region_is_free(
   const struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 ptr_start,
   Tux64UInt32 ptr_end
) {
   Tux64UInt8 i;
   const struct Tux64BootStage1ArenaAllocatorBlock * block;

   i = allocator->blocks_allocated;
   block = allocator->blocks;
   while (i != TUX64_LITERAL_UINT8(0u)) {
      if (tux64_boot_stage1_arena_allocator_region_excludes_block(
         block,
         ptr_start,
         ptr_end
      ) == TUX64_BOOLEAN_FALSE) {
         return TUX64_BOOLEAN_FALSE;
      }

      i--;
      block++;
   }

   return TUX64_BOOLEAN_TRUE;
}

Tux64Boolean
tux64_boot_stage1_arena_allocator_alloc_inplace(
   struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 address,
   Tux64UInt32 bytes
) {
   Tux64UInt32 ptr_start;
   Tux64UInt32 ptr_end;
   struct Tux64BootStage1ArenaAllocatorBlock * block_new;

   ptr_start   = address;
   ptr_end     = ptr_start + bytes;

   if (tux64_boot_stage1_arena_allocator_region_is_within_address_space(
      allocator,
      ptr_start,
      ptr_end
   ) == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOLEAN_FALSE;
   }

   if (tux64_boot_stage1_arena_allocator_region_is_free(
      allocator,
      ptr_start,
      ptr_end
   ) == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOLEAN_FALSE;
   }

   block_new = tux64_boot_stage1_arena_allocator_push_block(allocator);

   block_new->start  = ptr_start;
   block_new->end    = ptr_end;
   return TUX64_BOOLEAN_TRUE;
}

static Tux64UInt32
tux64_boot_stage1_arena_allocator_free_space_start(
   const struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 address_base,
   Tux64UInt32 alignment
) {
   Tux64UInt32 address_start;
   Tux64UInt8 i;
   const struct Tux64BootStage1ArenaAllocatorBlock * block;
   Tux64Boolean address_overlaps_block;

   address_start = address_base;
   do {
      address_start = tux64_math_align_forward_uint32(address_start, alignment);

      /* since forward-aligning may push us into an existing allocation, we */
      /* need all this crap. */
      i                       = allocator->blocks_allocated;
      block                   = allocator->blocks;
      address_overlaps_block  = TUX64_BOOLEAN_FALSE;
      while (i != TUX64_LITERAL_UINT8(0u)) {
         if (
            address_start >= block->start &&
            address_start < block->end
         ) {
            address_overlaps_block = TUX64_BOOLEAN_TRUE;
            address_start = block->end;
            break;
         }

         i--;
         block++;
      }

      if (address_overlaps_block == TUX64_BOOLEAN_FALSE) {
         break;
      }
   } while (address_start != allocator->address_space_end);

   return address_start;
}

static Tux64UInt32
tux64_boot_stage1_arena_allocator_free_space_end(
   const struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 free_space_start 
) {
   Tux64UInt8 i;
   const struct Tux64BootStage1ArenaAllocatorBlock * block;
   Tux64UInt32 free_space_end;

   i              = allocator->blocks_allocated;
   block          = allocator->blocks;
   free_space_end = allocator->address_space_end;

   while (i != TUX64_LITERAL_UINT8(0u)) {
      if (
         block->start > free_space_start &&
         block->start < free_space_end
      ) {
         free_space_end = block->start;
      }

      i--;
      block++;
   }

   return free_space_end;
}

Tux64UInt32
tux64_boot_stage1_arena_allocator_alloc(
   struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 bytes,
   Tux64UInt32 alignment
) {
   Tux64UInt32 free_region_start;
   Tux64UInt32 free_region_end;
   Tux64UInt32 free_region_bytes;
   Tux64UInt32 alloc_start;
   Tux64UInt32 alloc_end;
   struct Tux64BootStage1ArenaAllocatorBlock * block_new;

   /* we implement this using the best fit algorithm.  a little "wasted" */
   /* performance is worth optimizing the contiguous free block size. */

   free_region_start = allocator->address_space_start;
   alloc_start       = TUX64_LITERAL_UINT32(0u);
   alloc_end         = TUX64_LITERAL_UINT32(0u);
   
   do {
      free_region_start = tux64_boot_stage1_arena_allocator_free_space_start(allocator, free_region_start, alignment);
      free_region_end   = tux64_boot_stage1_arena_allocator_free_space_end(allocator, free_region_start);
      free_region_bytes = free_region_end - free_region_start;

      if (
         (free_region_bytes >= bytes) &&
         (
            alloc_start == TUX64_LITERAL_UINT32(0u) ||
            free_region_bytes < alloc_end - alloc_start
         )
      ) {
         alloc_start = free_region_start;
         alloc_end   = free_region_end;
      }

      free_region_start = free_region_end;
   } while (free_region_end != allocator->address_space_end);

   if (alloc_start == TUX64_LITERAL_UINT32(0u)) {
      return TUX64_LITERAL_UINT32(0u);
   }

   block_new = tux64_boot_stage1_arena_allocator_push_block(allocator);

   block_new->start  = alloc_start;
   block_new->end    = alloc_start + bytes;
   return alloc_start;
}

