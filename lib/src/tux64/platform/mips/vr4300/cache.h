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
/* Cache operation target constants, for use with the 'cache' instruction.    */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_INSTRUCTION\
   (0x00u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA\
   (0x01u)

/*----------------------------------------------------------------------------*/
/* Cache operation type constants, for use with the 'cache' instruction.      */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_INVALIDATE\
   (0x00u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_WRITE_BACK_INVALIDATE\
   (0x00u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_LOAD_TAG\
   (0x01u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_STORE_TAG\
   (0x02u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_CREATE_DIRTY_EXCLUSIVE\
   (0x03u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_INVALIDATE\
   (0x04u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_WRITE_BACK_INVALIDATE\
   (0x05u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_FILL\
   (0x05u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_WRITE_BACK\
   (0x06u)

/*----------------------------------------------------------------------------*/
/* Constructs a cache operation literal for use with the 'cache' instruction. */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(target, operation)\
   ((target) | ((operation) << 2u))

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

