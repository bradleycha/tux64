/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/parse/string-integer.h - Header for string integer parser    */
/*    functions.                                                              */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PARSE_STRING_INTEGER_H
#define _TUX64_PARSE_STRING_INTEGER_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

enum Tux64ParseStringIntegerStatus {
   TUX64_PARSE_STRING_INTEGER_STATUS_OK,
   TUX64_PARSE_STRING_INTEGER_STATUS_INVALID_DIGIT,
   TUX64_PARSE_STRING_INTEGER_STATUS_OUT_OF_RANGE
};

struct Tux64ParseStringIntegerPayloadInvalidDigit {
   char character;
};

union Tux64ParseStringIntegerPayload {
   struct Tux64ParseStringIntegerPayloadInvalidDigit invalid_digit;
};

struct Tux64ParseStringIntegerResult {
   enum Tux64ParseStringIntegerStatus status;
   union Tux64ParseStringIntegerPayload payload;
};

/*----------------------------------------------------------------------------*/
/* Parses a hex string without its prefix.                                    */
/*----------------------------------------------------------------------------*/
struct Tux64ParseStringIntegerResult
tux64_parse_string_integer_hex_uint8(
   const struct Tux64String * string,
   Tux64UInt8 * output
);
struct Tux64ParseStringIntegerResult
tux64_parse_string_integer_hex_uint16(
   const struct Tux64String * string,
   Tux64UInt16 * output
);
struct Tux64ParseStringIntegerResult
tux64_parse_string_integer_hex_uint32(
   const struct Tux64String * string,
   Tux64UInt32 * output
);
struct Tux64ParseStringIntegerResult
tux64_parse_string_integer_hex_uint64(
   const struct Tux64String * string,
   Tux64UInt64 * output
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PARSE_STRING_INTEGER_H */

