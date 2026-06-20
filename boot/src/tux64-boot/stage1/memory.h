/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/memory.h - Header for memory management.        */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_MEMORY_H
#define _TUX64_BOOT_STAGE1_MEMORY_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/boot.h>
#include "tux64-boot/stage1/stack.h"

/*----------------------------------------------------------------------------*/
/* The amount of bytes from the start of RDRAM reserved by stage-1, not       */
/* including the stage-1 binary itself.                                       */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE1_MEMORY_RDRAM_RESERVED \
   ( \
      TUX64_BOOT_STAGE1_STACK_BYTES + \
      TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES_ALIGN16 \
   )

#if !TUX64_PREPROCESSOR_ONLY
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Initialize the memory manager.  All other functions must be called only    */
/* after this function has first been called.                                 */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_memory_initialize(
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_free
);

/*----------------------------------------------------------------------------*/
/* Queries the total amount of RDRAM in the system, used or free.             */
/*----------------------------------------------------------------------------*/
Tux64UInt32
tux64_boot_stage1_memory_total(void);

/*----------------------------------------------------------------------------*/
/* Queries the amount of RDRAM in the system not in use by stage-1.           */
/*----------------------------------------------------------------------------*/
Tux64UInt32
tux64_boot_stage1_memory_free(void);

/*----------------------------------------------------------------------------*/
/* Attempts to allocate a range of memory at a specified address from the     */
/* stage-1 heap.  This follows the same API as                                */
/* tux64_boot_arena_allocator_alloc_inplace().                                */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_memory_stage1_alloc_inplace(
   void * address,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Attempts to allocate a block of memory which satisfies the given size and  */
/* alignment from the stage-1 heap.  This follows the same API as             */
/* tux64_boot_arena_allocator_alloc().                                        */
/*----------------------------------------------------------------------------*/
void *
tux64_boot_stage1_memory_stage1_alloc(
   Tux64UInt32 bytes,
   Tux64UInt8 alignment
);

/*----------------------------------------------------------------------------*/
/* Attempts to allocate a range of memory at a specified address from the     */
/* stage-2 heap.  This follows the same API as                                */
/* tux64_boot_arena_allocator_alloc_inplace().                                */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_memory_stage2_alloc_inplace(
   void * address,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Attempts to allocate a block of memory which satisfies the given size and  */
/* alignment from the stage-2 heap.  This follows the same API as             */
/* tux64_boot_arena_allocator_alloc().                                        */
/*----------------------------------------------------------------------------*/
void *
tux64_boot_stage1_memory_stage2_alloc(
   Tux64UInt32 bytes,
   Tux64UInt8 alignment
);

/*----------------------------------------------------------------------------*/
#endif /* !TUX64_PREPROCESSOR_ONLY */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_MEMORY_H */

