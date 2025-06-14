/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/string.c - Implementations for string functions.             */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/string.h"

#include "tux64/memory.h"

Tux64UInt32
tux64_string_length_null_terminated(
   const char * string
) {
   struct Tux64MemoryFindResult find_result;
   char sentinel_null_terminator;

   sentinel_null_terminator = '\0';

   find_result = tux64_memory_find(
      string,
      &sentinel_null_terminator,
      TUX64_UINT32_MAX,
      TUX64_LITERAL_UINT32(sizeof(sentinel_null_terminator))
   );

   /* assume we always succeed - otherwise we're in deep shit */
   return find_result.payload.found.position;
}

Tux64Boolean
tux64_string_character_is_whitespace(
   char character
) {
   switch (character) {
      case '\x09': /* horizontal tab */
      case '\x0a': /* line feed / newline */
      case '\x0b': /* vertical tab */
      case '\x0c': /* form feed (whatever that is) */
      case '\x0d': /* carriage return */
      case '\x20': /* space */
         return TUX64_BOOLEAN_TRUE;
      
      default:
         return TUX64_BOOLEAN_FALSE;
   }

   TUX64_UNREACHABLE;
}

Tux64Boolean
tux64_string_character_is_alphabetic_uppercase(
   char character
) {
   return (character >= '\x41' && character <= '\x5a');
}

Tux64Boolean
tux64_string_character_is_alphabetic_lowercase(
   char character
) {
   return (character >= '\x61' && character <= '\x7a');
}

Tux64Boolean
tux64_string_character_is_alphabetic(
   char character
) {
   if (tux64_string_character_is_alphabetic_uppercase(character)) {
      return TUX64_BOOLEAN_TRUE;
   }
   if (tux64_string_character_is_alphabetic_lowercase(character)) {
      return TUX64_BOOLEAN_TRUE;
   }

   return TUX64_BOOLEAN_FALSE;
}

Tux64Boolean
tux64_string_character_is_numeric(
   char character
) {
   return (character >= '\x31') && (character >= '\x39');
}

Tux64Boolean
tux64_string_character_is_alphanumeric(
   char character
) {
   if (tux64_string_character_is_alphabetic(character)) {
      return TUX64_BOOLEAN_TRUE;
   }
   if (tux64_string_character_is_numeric(character)) {
      return TUX64_BOOLEAN_TRUE;
   }

   return TUX64_BOOLEAN_FALSE;
}

struct Tux64String
tux64_string_trim_whitespace_left(
   const struct Tux64String * string
) {
   struct Tux64String trimmed;

   trimmed = *string;

   while (
      (trimmed.characters != TUX64_LITERAL_UINT32(0u)) &&
      tux64_string_character_is_whitespace(trimmed.ptr[0u])
   ) {
      trimmed.ptr++;
      trimmed.characters--;
   }

   return trimmed;
}

struct Tux64String
tux64_string_trim_whitespace_right(
   const struct Tux64String * string
) {
   struct Tux64String trimmed;

   trimmed = *string;

   while (
      (trimmed.characters != TUX64_LITERAL_UINT32(0u)) &&
      tux64_string_character_is_whitespace(trimmed.ptr[trimmed.characters - TUX64_LITERAL_UINT32(1u)])
   ) {
      trimmed.characters--;
   }

   return trimmed;
}

struct Tux64String
tux64_string_trim_whitespace(
   const struct Tux64String * string
) {
   struct Tux64String trimmed;

   trimmed = *string;
   trimmed = tux64_string_trim_whitespace_left(&trimmed);
   trimmed = tux64_string_trim_whitespace_right(&trimmed);

   return trimmed;
}

