/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/math.h - Header for math functions.                          */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_MATH_H
#define _TUX64_MATH_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/*----------------------------------------------------------------------------*/
/* Aligns a value forward to a boundary on literals.  Only use this if you    */
/* can't use the non-preprocessor functions, for example in #define           */
/* statements.                                                                */
/*----------------------------------------------------------------------------*/
#define TUX64_MATH_ALIGN_FORWARD(value, alignment)\
   (value + alignment - (value % alignment))

/*----------------------------------------------------------------------------*/
/* Aligns a value forward to a boundary.                                      */
/*----------------------------------------------------------------------------*/
Tux64UInt8
tux64_math_align_forward_uint8(
   Tux64UInt8 value,
   Tux64UInt8 alignment
);
Tux64UInt16
tux64_math_align_forward_uint16(
   Tux64UInt16 value,
   Tux64UInt16 alignment
);
Tux64UInt32
tux64_math_align_forward_uint32(
   Tux64UInt32 value,
   Tux64UInt32 alignment
);
Tux64UInt64
tux64_math_align_forward_uint64(
   Tux64UInt64 value,
   Tux64UInt64 alignment
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MATH_H */

