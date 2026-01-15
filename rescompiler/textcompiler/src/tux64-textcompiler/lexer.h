/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompilertextcompiler/src/tux64-textcompiler/lexer.h - Header for text   */
/*    file parser.                                                            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_TEXTCOMPILER_LEXER_H
#define _TUX64_TEXTCOMPILER_LEXER_H
/*----------------------------------------------------------------------------*/

#include "tux64-textcompiler/tux64-textcompiler.h"

struct Tux64TextCompilerLexerStringIdentifiers {
   struct Tux64String data;
   struct Tux64String length;
};

struct Tux64TextCompilerLexerText {
   const char * ptr;
   Tux64UInt8 characters;
};

struct Tux64TextCompilerLexerString {
   struct Tux64TextCompilerLexerStringIdentifiers identifiers;
   struct Tux64TextCompilerLexerText text;
};

struct Tux64TextCompilerLexerStringArray {
   struct Tux64TextCompilerLexerString * ptr;
   Tux64UInt32 length;
   Tux64UInt32 capacity;
};

struct Tux64TextCompilerLexerLocation {
   Tux64UInt32 line;
   Tux64UInt32 column;
};

enum Tux64TextCompilerLexerParseStatus {
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OUT_OF_MEMORY,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_EMPTY_FILE,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_START_INVALID,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_END_UNMATCHED,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_UNTERMINATED,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_UNTERMINATED,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_MALFORMED,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_IDENTIFIER,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_VALUE,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_SEPARATOR,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_NEWLINE,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_INCORRECT_TYPE_IDENTIFIER,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_INCORRECT_TYPE_TEXT,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_EMPTY,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_TOO_LONG,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_UNTERMINATED,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_BAD_CHARACTER,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_UNKNOWN_FIELD,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MALFORMED_FIELD,
   TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MISSING_REQUIRED_FIELD
};

struct Tux64TextCompilerLexerParsePayloadErr {
   struct Tux64TextCompilerLexerLocation location;
};

union Tux64TextCompilerLexerParsePayload {
   struct Tux64TextCompilerLexerStringArray ok;
   struct Tux64TextCompilerLexerParsePayloadErr err;
};

struct Tux64TextCompilerLexerParseResult {
   enum Tux64TextCompilerLexerParseStatus status;
   union Tux64TextCompilerLexerParsePayload payload;
};

/*----------------------------------------------------------------------------*/
/* Parses the given text file into its in-memory representation.  The         */
/* returned result, for all variants, has the same lifetime as the text file  */
/* string.  If the 'ok' variant is returned, it must be manually freed with   */
/* tux64_textcompiler_lexer_string_array_free().                              */
/*----------------------------------------------------------------------------*/
struct Tux64TextCompilerLexerParseResult
tux64_textcompiler_lexer_parse(
   const struct Tux64String * text
);

/*----------------------------------------------------------------------------*/
/* Frees allocated memory from the given string array.                        */
/*----------------------------------------------------------------------------*/
void
tux64_textcompiler_lexer_string_array_free(
   struct Tux64TextCompilerLexerStringArray * string_array
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_TEXTCOMPILER_LEXER_H */

