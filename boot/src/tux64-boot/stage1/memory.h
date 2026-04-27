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
/* Queries the amount of RDRAM in the system not in use.                      */
/*----------------------------------------------------------------------------*/
Tux64UInt32
tux64_boot_stage1_memory_free(void);

/*----------------------------------------------------------------------------*/
/* Queries the start address of free memory, returning a pointer to cached    */
/* RDRAM.                                                                     */
/*----------------------------------------------------------------------------*/
void *
tux64_boot_stage1_memory_top(void);

/*----------------------------------------------------------------------------*/
/* Checks if a given region of memory is free.  The region of memory must     */
/* reside in either the cached or uncached RDRAM segment.                     */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_memory_region_is_free(
   const void * start,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_MEMORY_H */

