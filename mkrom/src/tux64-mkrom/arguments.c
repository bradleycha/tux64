/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/arguments.c - Implementations for arguments parsing. */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/arguments.h"

struct Tux64MkromArgumentsParseResult
tux64_mkrom_arguments_parse(
   int argc,
   char ** argv,
   struct Tux64MkromArguments * output
) {
   struct Tux64MkromArgumentsParseResult result;

   /* TODO: implement */
   (void)argc;
   (void)argv;
   (void)output;
   
   result.status = TUX64_MKROM_ARGUMENTS_PARSE_STATUS_OUT_OF_MEMORY;
   return result;
}

