/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* fontcompiler/src/tux64-fontcompiler/arguments.h - Header for arguments     */
/*    parsing.                                                                */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_FONTCOMPILER_ARGUMENTS_H
#define _TUX64_FONTCOMPILER_ARGUMENTS_H
/*----------------------------------------------------------------------------*/

#include "tux64-fontcompiler/tux64-fontcompiler.h"
#include <tux64/arguments.h>

struct Tux64FontCompilerArguments {
   struct Tux64String path_input;
   struct Tux64String path_output;
   struct Tux64String name;
};

struct Tux64ArgumentsParseResult
tux64_fontcompiler_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64FontCompilerArguments * output
);

void
tux64_fontcompiler_arguments_print_menu_help(void);

void
tux64_fontcompiler_arguments_print_menu_version(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_FONTCOMPILER_ARGUMENTS_H */

