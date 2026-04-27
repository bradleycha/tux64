/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
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

#define TUX64_ARRAY_ELEMENTS(array)\
   (sizeof(array) / sizeof(array[0]))
#define TUX64_STRING_CHARACTERS(string)\
   (TUX64_ARRAY_ELEMENTS(string) - 1u)

#define TUX64_OFFSET_OF(type, member)\
   ((Tux64UIntPtr)(&((const type *)(TUX64_NULLPTR))->member))

/* this macro is used to declare a typeless address which is defined by the   */
/* linker.  we can't just use "void *" because it can result in weird,        */
/* incorrect, and unexplainable behavior.  we also can't just use "void"      */
/* because it's technically undefined behavior.  gcc will eat it (and usually */
/* generate correct code), but it will emit a warning and it's unreliable.    */
/* this is the only way i've found to reliably declare such symbols.          */
#define TUX64_LINKER_SYMBOL(identifier) \
   extern Tux64UInt8 identifier []

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_H */

