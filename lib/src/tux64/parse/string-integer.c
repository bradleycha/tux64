/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/parse/string-integer.c - Implementations for string integer  */
/*    parser functions.                                                       */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/parse/string-integer.h"

#include "tux64/bitwise.h"

struct Tux64ParseStringIntegerResult
tux64_parse_string_integer_hex_uint8(
   const struct Tux64String * string,
   Tux64UInt8 * output
) {
   struct Tux64ParseStringIntegerResult result;
   Tux64UInt64 value;

   result = tux64_parse_string_integer_hex_uint64(string, &value);
   if (result.status != TUX64_PARSE_STRING_INTEGER_STATUS_OK) {
      return result;
   }

   if (value > TUX64_UINT8_MAX) {
      result.status = TUX64_PARSE_STRING_INTEGER_STATUS_OUT_OF_RANGE;
      return result;
   }

   *output = (Tux64UInt8)value;
   result.status = TUX64_PARSE_STRING_INTEGER_STATUS_OK;
   return result;
}

struct Tux64ParseStringIntegerResult
tux64_parse_string_integer_hex_uint16(
   const struct Tux64String * string,
   Tux64UInt16 * output
) {
   struct Tux64ParseStringIntegerResult result;
   Tux64UInt64 value;

   result = tux64_parse_string_integer_hex_uint64(string, &value);
   if (result.status != TUX64_PARSE_STRING_INTEGER_STATUS_OK) {
      return result;
   }

   if (value > TUX64_UINT16_MAX) {
      result.status = TUX64_PARSE_STRING_INTEGER_STATUS_OUT_OF_RANGE;
      return result;
   }

   *output = (Tux64UInt16)value;
   result.status = TUX64_PARSE_STRING_INTEGER_STATUS_OK;
   return result;
}

struct Tux64ParseStringIntegerResult
tux64_parse_string_integer_hex_uint32(
   const struct Tux64String * string,
   Tux64UInt32 * output
) {
   struct Tux64ParseStringIntegerResult result;
   Tux64UInt64 value;

   result = tux64_parse_string_integer_hex_uint64(string, &value);
   if (result.status != TUX64_PARSE_STRING_INTEGER_STATUS_OK) {
      return result;
   }

   if (value > TUX64_UINT32_MAX) {
      result.status = TUX64_PARSE_STRING_INTEGER_STATUS_OUT_OF_RANGE;
      return result;
   }

   *output = (Tux64UInt32)value;
   result.status = TUX64_PARSE_STRING_INTEGER_STATUS_OK;
   return result;
}

static Tux64Boolean
tux64_parse_string_integer_hex_digit(
   char digit,
   Tux64UInt8 * output
) {
   if (digit >= '0' && digit <= '9') {
      *output = digit - TUX64_LITERAL_UINT8('0');
      return TUX64_BOOLEAN_TRUE;
   }
   if (digit >= 'a' && digit <= 'f') {
      *output = digit - TUX64_LITERAL_UINT8('a') + TUX64_LITERAL_UINT8(10);
      return TUX64_BOOLEAN_TRUE;
   }
   if (digit >= 'A' && digit <= 'F') {
      *output = digit - TUX64_LITERAL_UINT8('A') + TUX64_LITERAL_UINT8(10);
      return TUX64_BOOLEAN_TRUE;
   }

   return TUX64_BOOLEAN_FALSE;
}

struct Tux64ParseStringIntegerResult
tux64_parse_string_integer_hex_uint64(
   const struct Tux64String * string,
   Tux64UInt64 * output
) {
   struct Tux64ParseStringIntegerResult result;
   Tux64UInt64 buffer;
   const char * iter_string;
   Tux64UInt32 characters_remaining;
   char digit;
   Tux64UInt8 hexit;

   buffer = TUX64_LITERAL_UINT64(0llu);
   
   iter_string = string->ptr;
   characters_remaining = string->characters;

   while (characters_remaining != TUX64_LITERAL_UINT32(0u)) {
      digit = *iter_string;

      if (tux64_parse_string_integer_hex_digit(
         digit,
         &hexit
      ) == TUX64_BOOLEAN_FALSE) {
         result.status = TUX64_PARSE_STRING_INTEGER_STATUS_INVALID_DIGIT;
         result.payload.invalid_digit.character = digit;
         return result;
      }

      /* checks 4 most-significant bits if they're set to see if we're */
      /* risking overflow */
      if (tux64_bitwise_flags_check_one_uint64(
         buffer,
         TUX64_LITERAL_UINT64(0xf000000000000000llu)
      ) == TUX64_BOOLEAN_TRUE) {
         result.status = TUX64_PARSE_STRING_INTEGER_STATUS_OUT_OF_RANGE;
         return result;
      }

      buffer <<= TUX64_LITERAL_UINT8(4);
      buffer |= hexit;

      iter_string++;
      characters_remaining--;
   }

   *output = buffer;

   result.status = TUX64_PARSE_STRING_INTEGER_STATUS_OK;
   return result;
}

