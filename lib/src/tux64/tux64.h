/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/tux64.h - Main global header for Tux64.                      */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_H
#define _TUX64_H
/*------------------------------------------------------------------------------*/

/* this must be included first so all future includes have this available*/
#include "tux64/config.h"

/* i tried using AC_C_BIGENDIAN, but it fails to detect endianess for mips64 */
#ifndef TUX64_PLATFORM_CPU_ENDIAN_NATIVE_BIG
#ifndef __BYTE_ORDER__
#error "unable to detect the target platform CPU's endianess.  try manually defining TUX64_PLATFORM_CPU_ENDIAN_NATIVE_BIG if all else fails."
#endif /* __BYTE_ORDER__ */
#define TUX64_PLATFORM_CPU_ENDIAN_NATIVE_BIG\
   (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#endif /* TUX64_PLATFORM_CPU_ENDIAN_NATIVE_BIG */

#include "tux64/types.h"

#define TUX64_NULLPTR_SENTINEL\
   (0u)
#define TUX64_NULLPTR\
   ((void *)(TUX64_NULLPTR_SENTINEL))

#define TUX64_HALT\
   {for (;;) {}}
#define TUX64_UNREACHABLE\
   __builtin_unreachable()

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_H */

