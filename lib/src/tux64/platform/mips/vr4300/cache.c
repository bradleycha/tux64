/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/vr4300/cache.c - Implementations for VR4300    */
/*    cache management functions.                                             */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/vr4300/cache.h"

#include "tux64/platform/mips/vr4300/vr4300.h"

#if TUX64_PLATFORM_MIPS_VR4300_ENABLE
/*----------------------------------------------------------------------------*/

/* the number of bytes per line of cache */
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_INSTRUCTION\
   (32u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_DATA\
   (16u)

/* the total number of cache lines */
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_LINE_COUNT_INSTRUCTION\
   (512u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_LINE_COUNT_DATA\
   (512u)

/*----------------------------------------------------------------------------*/
#endif /* TUX64_PLATFORM_MIPS_VR4300_ENABLE */

