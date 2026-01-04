/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* textcompiler/src/tux64-textcompiler/formatter.h - Header for writing a     */
/*    parsed text file as formatted C source code.                            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_TEXTCOMPILER_FORMATTER_H
#define _TUX64_TEXTCOMPILER_FORMATTER_H
/*----------------------------------------------------------------------------*/

#include "tux64-textcompiler/tux64-textcompiler.h"
#include "tux64-textcompiler/lexer.h"

enum Tux64TextCompilerFormatterGenerateStatus {
   TUX64_TEXTCOMPILER_FORMATTER_GENERATE_STATUS_OK,
   TUX64_TEXTCOMPILER_FORMATTER_GENERATE_STATUS_OUT_OF_MEMORY
};

struct Tux64TextCompilerFormatterGeneratePayloadOk {
   Tux64UInt8 * data;
   Tux64UInt32 bytes;
};

union Tux64TextCompilerFormatterGeneratePayload {
   struct Tux64TextCompilerFormatterGeneratePayloadOk ok;
};

struct Tux64TextCompilerFormatterGenerateResult {
   enum Tux64TextCompilerFormatterGenerateStatus status;
   union Tux64TextCompilerFormatterGeneratePayload payload;
};

/*----------------------------------------------------------------------------*/
/* Generates C source code from the parsed text file.  If the 'ok' variant is */
/* returned, then it must be manually freed with free().                      */
/*----------------------------------------------------------------------------*/
struct Tux64TextCompilerFormatterGenerateResult
tux64_textcompiler_formatter_generate(
   const struct Tux64TextCompilerLexerStringArray * string_array
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_TEXTCOMPILER_FORMATTER_H */

