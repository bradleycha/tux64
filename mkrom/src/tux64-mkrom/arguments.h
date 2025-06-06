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

struct Tux64MkromArguments {
   const char * path_bootloader_stage0;
   const char * path_bootloader_stage0_cic;
   const char * path_bootloader_stage1;
   const char * path_bootloader_stage2;
   const char * path_bootloader_stage2_bss;
   const char * path_bootloader_stage3;
   const char * path_kernel;
   const char * path_initramfs;
   const char * command_line;
};

enum Tux64MkromArgumentsParseStatus {
   TUX64_MKROM_ARGUMENTS_PARSE_STATUS_OK,
   TUX64_MKROM_ARGUMENTS_PARSE_STATUS_EXIT,
   TUX64_MKROM_ARGUMENTS_PARSE_STATUS_OUT_OF_MEMORY,
   TUX64_MKROM_ARGUMENTS_PARSE_STATUS_INVALID
};

struct Tux64MkromArgumentsParsePayloadInvalid {
   const char * argument;
   const char * parameter;
};

union Tux64MkromArgumentsParsePayload {
   struct Tux64MkromArgumentsParsePayloadInvalid invalid;
};

struct Tux64MkromArgumentsParseResult {
   enum Tux64MkromArgumentsParseStatus status;
   union Tux64MkromArgumentsParsePayload payload;
};

struct Tux64MkromArgumentsParseResult
tux64_mkrom_arguments_parse(
   int argc,
   char ** argv,
   struct Tux64MkromArguments * output
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MKROM_ARGUMENTS_H */

