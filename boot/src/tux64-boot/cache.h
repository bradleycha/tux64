/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/cache.h - Header for cache management.                 */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_CACHE_H
#define _TUX64_BOOT_CACHE_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Invalidate instruction cache in the range specified.                       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_cache_invalidate_instruction(
   const void * address,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Invalidate data cache in the range specified.                              */
/*----------------------------------------------------------------------------*/
void
tux64_boot_cache_invalidate_data(
   const void * address,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Invalidate both instruction cache and data cache in the range specified.   */
/*----------------------------------------------------------------------------*/
void
tux64_boot_cache_invalidate(
   const void * address,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_CACHE_H */

