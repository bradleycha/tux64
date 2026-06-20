/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/arena-allocator.h - Header for implementing an arena   */
/*    allocator.                                                              */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_ARENA_ALLOCATOR_H
#define _TUX64_BOOT_ARENA_ALLOCATOR_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/* kernel + initramfs + command line.  this is why we don't implement this in */
/* tux64-lib. */
#define TUX64_BOOT_ARENA_ALLOCATOR_MAX_ALLOCATIONS\
   3u

struct Tux64BootArenaAllocatorBlock {
   Tux64UIntPtr start;
   Tux64UIntPtr end;
};

/*----------------------------------------------------------------------------*/
/* Stores the information to implement an arena allocator.                    */
/*----------------------------------------------------------------------------*/
struct Tux64BootArenaAllocator {
   Tux64UIntPtr address_space_start;
   Tux64UIntPtr address_space_end;
   struct Tux64BootArenaAllocatorBlock blocks [TUX64_BOOT_ARENA_ALLOCATOR_MAX_ALLOCATIONS];
   Tux64UInt8 blocks_allocated;
};

/*----------------------------------------------------------------------------*/
/* Initializes an arena allocator.  All allocations will return addresses in  */
/* the range 'address_space_start' to 'address_space_end'.                    */
/*----------------------------------------------------------------------------*/
void
tux64_boot_arena_allocator_initialize(
   struct Tux64BootArenaAllocator * allocator,
   void * address_space_start,
   void * address_space_end
);

/*----------------------------------------------------------------------------*/
/* Attempts to allocate a range of memory at a specified address.  Returns    */
/* whether the allocation succeeded or not.  The given address range must be  */
/* within the heap's address space.                                           */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_arena_allocator_alloc_inplace(
   struct Tux64BootArenaAllocator * allocator,
   void * address,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Attempts to allocate a block of memory which satisfies the given size and  */
/* alignment.  Returns the pointer to the allocated block upon success, or    */
/* TUX64_NULLPTR upon failure.                                                */
/*----------------------------------------------------------------------------*/
void *
tux64_boot_arena_allocator_alloc(
   struct Tux64BootArenaAllocator * allocator,
   Tux64UInt32 bytes,
   Tux64UInt8 alignment
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_ARENA_ALLOCATOR_H */

