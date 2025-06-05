/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/endian.h - Header for endian functions.                      */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_ENDIAN_H
#define _TUX64_ENDIAN_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

enum Tux64EndianFormat {
   TUX64_ENDIAN_FORMAT_LITTLE,
   TUX64_ENDIAN_FORMAT_BIG
};

#if TUX64_PLATFORM_CPU_ENDIAN_NATIVE_BIG
#define TUX64_ENDIAN_FORMAT_NATIVE TUX64_ENDIAN_FORMAT_BIG
#else /* TUX64_PLATFORM_CPU_ENDIAN_NATIVE_BIG */
#define TUX64_ENDIAN_FORMAT_NATIVE TUX64_ENDIAN_FORMAT_LITTLE
#endif /* TUX64_PLATFORM_CPU_ENDIAN_NATIVE_BIG */

/*----------------------------------------------------------------------------*/
/* Converts endianess from the CPU's native endian format to the target       */
/* endian format.                                                             */
/*----------------------------------------------------------------------------*/
void
tux64_endian_convert_inplace(
   Tux64UInt8 * restrict data,
   Tux64UInt32 bytes,
   enum Tux64EndianFormat format
);
void
tux64_endian_convert_copy(
   Tux64UInt8 * restrict output,
   const Tux64UInt8 * restrict data,
   Tux64UInt32 bytes,
   enum Tux64EndianFormat format
);
Tux64UInt16
tux64_endian_convert_uint16(
   Tux64UInt16 value,
   enum Tux64EndianFormat format
);
Tux64UInt32
tux64_endian_convert_uint32(
   Tux64UInt32 value,
   enum Tux64EndianFormat format
);
Tux64UInt64
tux64_endian_convert_uint64(
   Tux64UInt64 value,
   enum Tux64EndianFormat format
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_ENDIAN_H */

