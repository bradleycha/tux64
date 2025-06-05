/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/endian.c - Implementations for endian functions.             */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/endian.h"

static void
tux64_endian_swap_inplace(
   Tux64UInt8 data [],
   Tux64UInt32 bytes
) {
   Tux64UInt8 * iter_start;
   Tux64UInt8 * iter_end;
   Tux64UInt8 temp;

   iter_start = data;
   iter_end = data + bytes - 1u;
   while (iter_start < iter_end) {
      temp = *iter_start;
      *iter_start = *iter_end;
      *iter_end = temp;

      iter_start++;
      iter_end--;
   }

   return;
}

static void
tux64_endian_swap_copy(
   const Tux64UInt8 data [],
   Tux64UInt8 output [],
   Tux64UInt32 bytes
) {
   const Tux64UInt8 * iter_data;
   Tux64UInt8 * iter_output;

   iter_data = data;
   iter_output = output + bytes - 1u;
   while (bytes != 0u) {
      *iter_output = *iter_data;

      iter_data++;
      iter_output--;
      bytes--;
   }

   return;
}

void
tux64_endian_convert_inplace(
   Tux64UInt8 data [],
   Tux64UInt32 bytes,
   enum Tux64EndianFormat format
) {
   if (format == TUX64_ENDIAN_FORMAT_NATIVE) {
      return;
   }

   tux64_endian_swap_inplace(data, bytes);
   return;
}

void
tux64_endian_convert_copy(
   const Tux64UInt8 data [],
   Tux64UInt8 output [],
   Tux64UInt32 bytes,
   enum Tux64EndianFormat format
) {
   if (format == TUX64_ENDIAN_FORMAT_NATIVE) {
      return;
   }

   tux64_endian_swap_copy(data, output, bytes);
   return;
}

Tux64UInt16
tux64_endian_convert_uint16(
   Tux64UInt16 value,
   enum Tux64EndianFormat format
) {
   union Tux64Int16 value_bytes;

   value_bytes.uint = value;

   tux64_endian_convert_inplace(
      value_bytes.bytes,
      sizeof(value_bytes.bytes),
      format
   );

   return value_bytes.uint;
}

Tux64UInt32
tux64_endian_convert_uint32(
   Tux64UInt32 value,
   enum Tux64EndianFormat format
) {
   union Tux64Int32 value_bytes;

   value_bytes.uint = value;

   tux64_endian_convert_inplace(
      value_bytes.bytes,
      sizeof(value_bytes.bytes),
      format
   );

   return value_bytes.uint;
}

Tux64UInt64
tux64_endian_convert_uint64(
   Tux64UInt64 value,
   enum Tux64EndianFormat format
) {
   union Tux64Int64 value_bytes;

   value_bytes.uint = value;

   tux64_endian_convert_inplace(
      value_bytes.bytes,
      sizeof(value_bytes.bytes),
      format
   );

   return value_bytes.uint;
}

