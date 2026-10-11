/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/string.h - Header for filesystem functions.                  */
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
/* Formats a null-terminated C-string into the given buffer.  The buffer must */
/* large enough to contain the length of the string, plus one extra character */
/* for the null-terminator.                                                   */
/*----------------------------------------------------------------------------*/
void
tux64_string_format_cstr(
   const struct Tux64String * string,
   char buffer[]
);

#define _TUX64_STRING_ALLOCATE_CSTR_ENABLE\
   (TUX64_HAVE_STDLIB_H)

#if _TUX64_STRING_ALLOCATE_CSTR_ENABLE
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Formats a null-terminated C-string into an allocated buffer.  The buffer   */
/* must be freed by the caller if allocation was successful.  If allocation   */
/* fails, then TUX64_NULLPTR is returned.  Depends on the availability of     */
/* stdlib.h for malloc().                                                     */
/*----------------------------------------------------------------------------*/
char *
tux64_string_allocate_and_format_cstr(
   const struct Tux64String * string
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_STRING_ALLOCATE_CSTR_ENABLE */


/*----------------------------------------------------------------------------*/
#endif /* _TUX64_STRING_H */

