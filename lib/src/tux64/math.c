/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/math.c - Implementations for math functions.                 */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/math.h"

Tux64UInt8
tux64_math_ceil_divide_uint8(
   Tux64UInt8 dividend,
   Tux64UInt8 divisor
) {
   return (Tux64UInt8)tux64_math_ceil_divide_uint64(
      (Tux64UInt64)dividend,
      (Tux64UInt64)divisor
   );
}

Tux64UInt16
tux64_math_ceil_divide_uint16(
   Tux64UInt16 dividend,
   Tux64UInt16 divisor
) {
   return (Tux64UInt16)tux64_math_ceil_divide_uint64(
      (Tux64UInt64)dividend,
      (Tux64UInt64)divisor
   );
}

Tux64UInt32
tux64_math_ceil_divide_uint32(
   Tux64UInt32 dividend,
   Tux64UInt32 divisor
) {
   return (Tux64UInt32)tux64_math_ceil_divide_uint64(
      (Tux64UInt64)dividend,
      (Tux64UInt64)divisor
   );
}

Tux64UInt64
tux64_math_ceil_divide_uint64(
   Tux64UInt64 dividend,
   Tux64UInt64 divisor
) {
   if (dividend == TUX64_LITERAL_UINT64(0llu)) {
      return TUX64_LITERAL_UINT64(0llu);
   }

   return TUX64_LITERAL_UINT64(1llu) + ((dividend - 1) / divisor);
}

Tux64UInt8
tux64_math_align_forward_uint8(
   Tux64UInt8 value,
   Tux64UInt8 alignment
) {
   return (Tux64UInt8)tux64_math_align_forward_uint64(
      (Tux64UInt64)value,
      (Tux64UInt64)alignment
   );
}

Tux64UInt16
tux64_math_align_forward_uint16(
   Tux64UInt16 value,
   Tux64UInt16 alignment
) {
   return (Tux64UInt16)tux64_math_align_forward_uint64(
      (Tux64UInt64)value,
      (Tux64UInt64)alignment
   );
}

Tux64UInt32
tux64_math_align_forward_uint32(
   Tux64UInt32 value,
   Tux64UInt32 alignment
) {
   return (Tux64UInt32)tux64_math_align_forward_uint64(
      (Tux64UInt64)value,
      (Tux64UInt64)alignment
   );
}

Tux64UInt64
tux64_math_align_forward_uint64(
   Tux64UInt64 value,
   Tux64UInt64 alignment
) {
   return tux64_math_ceil_divide_uint64(value, alignment) * alignment;
}

