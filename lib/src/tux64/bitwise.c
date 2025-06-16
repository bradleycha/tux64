/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/bitwise.c - Implementations for bitwise functions.           */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/bitwise.h"

#define TUX64_BITWISE_ROTATE_LEFT(bitwidth, value, bits)\
   (((value) << (bits)) | ((value) >> ((bitwidth) - (bits))))
#define TUX64_BITWISE_ROTATE_RIGHT(bitwidth, value, bits)\
   (((value) >> (bits)) | ((value) << ((bitwidth) - (bits))))

Tux64UInt8
tux64_bitwise_rotate_left_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_ROTATE_LEFT(sizeof(value) * 8u, value, bits);
}

Tux64UInt16
tux64_bitwise_rotate_left_uint16(
   Tux64UInt16 value,
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_ROTATE_LEFT(sizeof(value) * 8u, value, bits);
}

Tux64UInt32
tux64_bitwise_rotate_left_uint32(
   Tux64UInt32 value,
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_ROTATE_LEFT(sizeof(value) * 8u, value, bits);
}

Tux64UInt64
tux64_bitwise_rotate_left_uint64(
   Tux64UInt64 value,
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_ROTATE_LEFT(sizeof(value) * 8u, value, bits);
}

Tux64UInt8
tux64_bitwise_rotate_right_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_ROTATE_RIGHT(sizeof(value) * 8u, value, bits);
}

Tux64UInt16
tux64_bitwise_rotate_right_uint16(
   Tux64UInt16 value,
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_ROTATE_RIGHT(sizeof(value) * 8u, value, bits);
}

Tux64UInt32
tux64_bitwise_rotate_right_uint32(
   Tux64UInt32 value,
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_ROTATE_RIGHT(sizeof(value) * 8u, value, bits);
}

Tux64UInt64
tux64_bitwise_rotate_right_uint64(
   Tux64UInt64 value,
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_ROTATE_RIGHT(sizeof(value) * 8u, value, bits);
}

#define TUX64_BITWISE_FLAGS_SET(value, bit_flags)\
   ((value) | (bit_flags))

Tux64UInt8
tux64_bitwise_flags_set_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
) {
   return TUX64_BITWISE_FLAGS_SET(value, bit_flags);
}

Tux64UInt16
tux64_bitwise_flags_set_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
) {
   return TUX64_BITWISE_FLAGS_SET(value, bit_flags);
}

Tux64UInt32
tux64_bitwise_flags_set_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
) {
   return TUX64_BITWISE_FLAGS_SET(value, bit_flags);
}

Tux64UInt64
tux64_bitwise_flags_set_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
) {
   return TUX64_BITWISE_FLAGS_SET(value, bit_flags);
}

#define TUX64_BITWISE_FLAGS_CLEAR(value, bit_flags)\
   ((value) & (~(bit_flags)))

Tux64UInt8
tux64_bitwise_flags_clear_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CLEAR(value, bit_flags);
}

Tux64UInt16
tux64_bitwise_flags_clear_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CLEAR(value, bit_flags);
}

Tux64UInt32
tux64_bitwise_flags_clear_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CLEAR(value, bit_flags);
}

Tux64UInt64
tux64_bitwise_flags_clear_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CLEAR(value, bit_flags);
}

#define TUX64_BITWISE_FLAGS_FLIP(value, bit_flags)\
   ((value) ^ (bit_flags))

Tux64UInt8
tux64_bitwise_flags_flip_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
) {
   return TUX64_BITWISE_FLAGS_FLIP(value, bit_flags);
}

Tux64UInt16
tux64_bitwise_flags_flip_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
) {
   return TUX64_BITWISE_FLAGS_FLIP(value, bit_flags);
}

Tux64UInt32
tux64_bitwise_flags_flip_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
) {
   return TUX64_BITWISE_FLAGS_FLIP(value, bit_flags);
}

Tux64UInt64
tux64_bitwise_flags_flip_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
) {
   return TUX64_BITWISE_FLAGS_FLIP(value, bit_flags);
}

#define TUX64_BITWISE_FLAGS_CHECK_ONE(value, bit_flags)\
   (((value) & (bit_flags)) != 0u ? TUX64_BOOLEAN_TRUE : TUX64_BOOLEAN_FALSE)

Tux64Boolean
tux64_bitwise_flags_check_one_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CHECK_ONE(value, bit_flags);
}

Tux64Boolean
tux64_bitwise_flags_check_one_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CHECK_ONE(value, bit_flags);
}

Tux64Boolean
tux64_bitwise_flags_check_one_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CHECK_ONE(value, bit_flags);
}

Tux64Boolean
tux64_bitwise_flags_check_one_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CHECK_ONE(value, bit_flags);
}

#define TUX64_BITWISE_FLAGS_CHECK_ALL(value, bit_flags)\
   (((value) & (bit_flags)) == (value) ? TUX64_BOOLEAN_TRUE : TUX64_BOOLEAN_FALSE)

Tux64Boolean
tux64_bitwise_flags_check_all_uint8(
   Tux64UInt8 value,
   Tux64UInt8 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CHECK_ALL(value, bit_flags);
}

Tux64Boolean
tux64_bitwise_flags_check_all_uint16(
   Tux64UInt16 value,
   Tux64UInt16 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CHECK_ALL(value, bit_flags);
}

Tux64Boolean
tux64_bitwise_flags_check_all_uint32(
   Tux64UInt32 value,
   Tux64UInt32 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CHECK_ALL(value, bit_flags);
}

Tux64Boolean
tux64_bitwise_flags_check_all_uint64(
   Tux64UInt64 value,
   Tux64UInt64 bit_flags
) {
   return TUX64_BITWISE_FLAGS_CHECK_ALL(value, bit_flags);
}

#define TUX64_BITWISE_MASK_SET_ALL_LOW(bits, one_sentinel)\
   (((one_sentinel) << (bits)) - (one_sentinel))

Tux64UInt8
tux64_bitwise_mask_set_all_low_uint8(
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_MASK_SET_ALL_LOW(bits, TUX64_LITERAL_UINT8(1u));
}

Tux64UInt16
tux64_bitwise_mask_set_all_low_uint16(
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_MASK_SET_ALL_LOW(bits, TUX64_LITERAL_UINT16(1u));
}

Tux64UInt32
tux64_bitwise_mask_set_all_low_uint32(
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_MASK_SET_ALL_LOW(bits, TUX64_LITERAL_UINT32(1u));
}

Tux64UInt64
tux64_bitwise_mask_set_all_low_uint64(
   Tux64UInt8 bits
) {
   return TUX64_BITWISE_MASK_SET_ALL_LOW(bits, TUX64_LITERAL_UINT64(1u));
}

