/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/bitwise.h - Header for bitwise functions.                    */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BITWISE_H
#define _TUX64_BITWISE_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/*----------------------------------------------------------------------------*/
/* Performs a bitwise rotation of 'value' by 'bits' number of bits.           */
/*----------------------------------------------------------------------------*/
Tux64UInt8
tux64_bitwise_rotate_left_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bits
);
Tux64UInt16
tux64_bitwise_rotate_left_uint16(
   Tux64UInt16 value,
   Tux64UInt8 bits
);
Tux64UInt32
tux64_bitwise_rotate_left_uint32(
   Tux64UInt32 value,
   Tux64UInt8 bits
);
Tux64UInt64
tux64_bitwise_rotate_left_uint64(
   Tux64UInt64 value,
   Tux64UInt8 bits
);
Tux64UInt8
tux64_bitwise_rotate_right_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bits
);
Tux64UInt16
tux64_bitwise_rotate_right_uint16(
   Tux64UInt16 value,
   Tux64UInt8 bits
);
Tux64UInt32
tux64_bitwise_rotate_right_uint32(
   Tux64UInt32 value,
   Tux64UInt8 bits
);
Tux64UInt64
tux64_bitwise_rotate_right_uint64(
   Tux64UInt64 value,
   Tux64UInt8 bits
);

/*----------------------------------------------------------------------------*/
/* Set all bit-flags in 'value' according to 'bit_flags'.                     */
/*----------------------------------------------------------------------------*/
Tux64UInt8
tux64_bitwise_flags_set_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
);
Tux64UInt16
tux64_bitwise_flags_set_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
);
Tux64UInt32
tux64_bitwise_flags_set_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
);
Tux64UInt64
tux64_bitwise_flags_set_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
);

/*----------------------------------------------------------------------------*/
/* Clears all bit-flags in 'value' according to 'bit_flags'.                  */
/*----------------------------------------------------------------------------*/
Tux64UInt8
tux64_bitwise_flags_clear_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
);
Tux64UInt16
tux64_bitwise_flags_clear_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
);
Tux64UInt32
tux64_bitwise_flags_clear_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
);
Tux64UInt64
tux64_bitwise_flags_clear_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
);

/*----------------------------------------------------------------------------*/
/* Flips all bit-flags in 'value' according to 'bit_flags'.                   */
/*----------------------------------------------------------------------------*/
Tux64UInt8
tux64_bitwise_flags_flip_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
);
Tux64UInt16
tux64_bitwise_flags_flip_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
);
Tux64UInt32
tux64_bitwise_flags_flip_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
);
Tux64UInt64
tux64_bitwise_flags_flip_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
);

/*----------------------------------------------------------------------------*/
/* Checks if at least of the bit flags in 'bit_flags' is set in 'value'.      */
/*----------------------------------------------------------------------------*/
Tux64UInt8
tux64_bitwise_flags_check_one_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
);
Tux64UInt16
tux64_bitwise_flags_check_one_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
);
Tux64UInt32
tux64_bitwise_flags_check_one_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
);
Tux64UInt64
tux64_bitwise_flags_check_one_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
);

/*----------------------------------------------------------------------------*/
/* Checks if all of the bit flags in 'bit_flags' are set in 'value'.          */
/*----------------------------------------------------------------------------*/
Tux64UInt8
tux64_bitwise_flags_check_all_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
);
Tux64UInt16
tux64_bitwise_flags_check_all_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
);
Tux64UInt32
tux64_bitwise_flags_check_all_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
);
Tux64UInt64
tux64_bitwise_flags_check_all_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BITWISE_H */

