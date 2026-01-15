/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/textcompiler/src/tux64-textcompiler/arguments.h - Header for   */
/*    arguments parsing.                                                      */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_TEXTCOMPILER_ARGUMENTS_H
#define _TUX64_TEXTCOMPILER_ARGUMENTS_H
/*----------------------------------------------------------------------------*/

#include "tux64-textcompiler/tux64-textcompiler.h"
#include <tux64/arguments.h>

struct Tux64TextCompilerArguments {
   struct Tux64String path_input;
   struct Tux64String path_output;
};

struct Tux64ArgumentsParseResult
tux64_textcompiler_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64TextCompilerArguments * output
);

void
tux64_textcompiler_arguments_print_menu_help(void);

void
tux64_textcompiler_arguments_print_menu_version(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_TEXTCOMPILER_ARGUMENTS_H */

