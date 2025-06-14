/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/string.h - Header for filesystem functions.                      */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_STRING_H
#define _TUX64_STRING_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/*----------------------------------------------------------------------------*/
/* Finds the length of a null-terminated C-style string.                      */
/*----------------------------------------------------------------------------*/
Tux64UInt32
tux64_string_length_null_terminated(
   const char * string
);

/*----------------------------------------------------------------------------*/
/* Tells whether a character can fit into a category or not.                  */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_string_character_is_whitespace(
   char character   
);
Tux64Boolean
tux64_string_character_is_alphabetic_uppercase(
   char character   
);
Tux64Boolean
tux64_string_character_is_alphabetic_lowercase(
   char character   
);
Tux64Boolean
tux64_string_character_is_alphabetic(
   char character   
);
Tux64Boolean
tux64_string_character_is_numeric(
   char character   
);
Tux64Boolean
tux64_string_character_is_alphanumeric(
   char character   
);

/*----------------------------------------------------------------------------*/
/* Trims whitespace from a string.                                            */
/*----------------------------------------------------------------------------*/
struct Tux64String
tux64_string_trim_whitespace_left(
   const struct Tux64String * string
);
struct Tux64String
tux64_string_trim_whitespace_right(
   const struct Tux64String * string
);
struct Tux64String
tux64_string_trim_whitespace(
   const struct Tux64String * string
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_STRING_H */

