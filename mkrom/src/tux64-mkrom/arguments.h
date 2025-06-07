/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/arguments.h - Header for arguments parsing.          */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_MKROM_ARGUMENTS_H
#define _TUX64_MKROM_ARGUMENTS_H
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include <tux64/arguments.h>

struct Tux64MkromArguments {
   const char * path_configuration;
   const char * path_output;
   const char * path_prefix;
};

struct Tux64ArgumentsParseResult
tux64_mkrom_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64MkromArguments * output
);

void
tux64_mkrom_arguments_print_menu_help(void);

void
tux64_mkrom_arguments_print_menu_version(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MKROM_ARGUMENTS_H */

