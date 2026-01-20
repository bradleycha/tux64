/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/imagecompiler/src/tux64-imagecompiler/arguments.h - Header for */
/*    arguments parsing.                                                      */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_IMAGECOMPILER_ARGUMENTS_H
#define _TUX64_IMAGECOMPILER_ARGUMENTS_H
/*----------------------------------------------------------------------------*/

#include "tux64-imagecompiler/tux64-imagecompiler.h"
#include <tux64/arguments.h>

struct Tux64ImageCompilerArguments {
   struct Tux64String path_input;
   struct Tux64String path_output;
   struct Tux64String name_pixels;
   struct Tux64String name_color_table;
};

struct Tux64ArgumentsParseResult
tux64_imagecompiler_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64ImageCompilerArguments * output
);

void
tux64_imagecompiler_arguments_print_menu_help(void);

void
tux64_imagecompiler_arguments_print_menu_version(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_IMAGECOMPILER_ARGUMENTS_H */

