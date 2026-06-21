/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/arena-allocator.h - Header for implementing an  */
/*    arena allocator.                                                        */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_ARENA_ALLOCATOR_H
#define _TUX64_BOOT_STAGE1_ARENA_ALLOCATOR_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/* kernel + kernel args + initramfs + command line.  this is why we don't */
/* implement this in tux64-lib. */
#define TUX64_BOOT_STAGE1_ARENA_ALLOCATOR_MAX_ALLOCATIONS\
   4u

struct Tux64BootStage1ArenaAllocatorBlock {
   Tux64UInt32 start;
   Tux64UInt32 end;
};

/*----------------------------------------------------------------------------*/
/* Stores the information to implement an arena allocator.                    */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1ArenaAllocator {
   Tux64UInt32 address_space_start;
   Tux64UInt32 address_space_end;
   struct Tux64BootStage1ArenaAllocatorBlock blocks [TUX64_BOOT_STAGE1_ARENA_ALLOCATOR_MAX_ALLOCATIONS];
   Tux64UInt8 blocks_allocated;
};

/*----------------------------------------------------------------------------*/
/* Initializes an arena allocator.  All allocations will return addresses in  */
/* the range 'address_space_start' to 'address_space_end'.                    */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_arena_allocator_initialize(
   struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 address_space_start,
   Tux64UInt32 address_space_end
);

/*----------------------------------------------------------------------------*/
/* Attempts to allocate a range of memory at a specified address.  Returns    */
/* whether the allocation succeeded or not.  The given address range must be  */
/* within the heap's address space.                                           */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_arena_allocator_alloc_inplace(
   struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 address,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Attempts to allocate a block of memory which satisfies the given size and  */
/* alignment.  Returns the pointer to the allocated block upon success, or    */
/* zero upon failure.                                                         */
/*----------------------------------------------------------------------------*/
Tux64UInt32
tux64_boot_stage1_arena_allocator_alloc(
   struct Tux64BootStage1ArenaAllocator * allocator,
   Tux64UInt32 bytes,
   Tux64UInt8 alignment
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_ARENA_ALLOCATOR_H */

