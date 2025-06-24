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
#include <tux64/platform/mips/n64/rom.h>

struct Tux64MkromArgumentsCommandLine {
   struct Tux64String path_config;
   struct Tux64String path_output;
   struct Tux64String path_prefix;
};

struct Tux64ArgumentsParseResult
tux64_mkrom_arguments_command_line_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64MkromArgumentsCommandLine * output
);

struct Tux64MkromArgumentsConfigFile {
   struct Tux64PlatformMipsN64RomHeader rom_header;
   struct Tux64String path_bootloader_stage0;
   struct Tux64String path_bootloader_stage0_cic;
   struct Tux64String path_bootloader_stage1;
   struct Tux64String path_bootloader_stage2;
   struct Tux64String path_bootloader_stage2_bss;
   struct Tux64String path_bootloader_stage3;
   struct Tux64String path_kernel;
   struct Tux64String path_initramfs;
   struct Tux64String command_line;
};

extern const struct Tux64ArgumentsIteratorOptionsConfigFile
tux64_mkrom_arguments_config_file_iterator_options;

struct Tux64ArgumentsParseResult
tux64_mkrom_arguments_config_file_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64MkromArgumentsConfigFile * output
);

void
tux64_mkrom_arguments_command_line_print_menu_help(void);

void
tux64_mkrom_arguments_command_line_print_menu_version(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MKROM_ARGUMENTS_H */

