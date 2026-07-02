/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-sramdumper/arguments.h - Header for arguments parsing.     */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_SRAMDUMPER_ARGUMENTS_H
#define _TUX64_SRAMDUMPER_ARGUMENTS_H
/*----------------------------------------------------------------------------*/

#include "tux64-sramdumper/tux64-sramdumper.h"
#include <tux64/arguments.h>

struct Tux64SramDumperArguments {
   struct Tux64String path_input;
   struct Tux64String path_output;
};

struct Tux64ArgumentsParseResult
tux64_sramdumper_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64SramDumperArguments * output
);

void
tux64_sramdumper_arguments_print_menu_help(void);

void
tux64_sramdumper_arguments_print_menu_version(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_SRAMDUMPER_ARGUMENTS_H */

