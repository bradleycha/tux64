/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/vr4300/cache.h - Header for VR4300 cache       */
/*    management functions.                                                   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_VR4300_CACHE_H
#define _TUX64_PLATFORM_MIPS_VR4300_CACHE_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/vr4300/vr4300.h"

#if TUX64_PLATFORM_MIPS_VR4300_ENABLE
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* The number of bytes per line of cache.                                     */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_INSTRUCTION\
   (32u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_DATA\
   (16u)

/*----------------------------------------------------------------------------*/
/* The total number of cache lines.                                           */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_LINE_COUNT_INSTRUCTION\
   (512u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_LINE_COUNT_DATA\
   (512u)

/*----------------------------------------------------------------------------*/
/* The raw cache instruction.  This should be avoided at all costs, using     */
/* wrapper functions if possible.  'address' is the address to perform the    */
/* cache instruction on, and 'operation' is the constant describing the cache */
/* operation to perform.  'address' can either be a pointer type or a         */
/* Tux64UIntPtr type containing the address to perform the instruction on.    */
/* 'operation' must be a 16-bit integer literal.                              */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(address, operation)\
   __asm__ volatile ( \
      "cache   %0,%1"\
      :: "K" (operation), "ZC" (*((const Tux64UInt8 *)(address)))\
   );

/*----------------------------------------------------------------------------*/
#endif /* TUX64_PLATFORM_MIPS_VR4300_ENABLE */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_VR4300_CACHE_H */

