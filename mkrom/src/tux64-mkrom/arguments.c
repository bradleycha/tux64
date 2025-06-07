/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/arguments.c - Implementations for arguments parsing. */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/arguments.h"

#include <tux64/arguments.h>
#include <stdio.h>

static enum Tux64ArgumentsParseStatus
tux64_mkrom_arguments_parser_path(
   const char * parameter,
   Tux64UInt32 parameter_characters,
   const char * * arguments_entry,
   Tux64UInt32 * arguments_entry_characters
) {
   if (parameter_characters == TUX64_LITERAL_UINT32(0u)) {
      return TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING;
   }

   *arguments_entry = parameter;
   *arguments_entry_characters = parameter_characters;
   return TUX64_ARGUMENTS_PARSE_STATUS_OK;
}

static enum Tux64ArgumentsParseStatus
tux64_mkrom_arguments_parser_config(
   const char * parameter,
   Tux64UInt32 parameter_characters,
   void * context
) {
   struct Tux64MkromArguments * arguments;

   arguments = (struct Tux64MkromArguments *)context;

   return tux64_mkrom_arguments_parser_path(
      parameter,
      parameter_characters,
      &arguments->path_config,
      &arguments->path_config_characters
   );
}

static enum Tux64ArgumentsParseStatus
tux64_mkrom_arguments_parser_output(
   const char * parameter,
   Tux64UInt32 parameter_characters,
   void * context
) {
   struct Tux64MkromArguments * arguments;

   arguments = (struct Tux64MkromArguments *)context;

   return tux64_mkrom_arguments_parser_path(
      parameter,
      parameter_characters,
      &arguments->path_output,
      &arguments->path_output_characters
   );
}

static enum Tux64ArgumentsParseStatus
tux64_mkrom_arguments_parser_prefix(
   const char * parameter,
   Tux64UInt32 parameter_characters,
   void * context
) {
   struct Tux64MkromArguments * arguments;

   arguments = (struct Tux64MkromArguments *)context;

   return tux64_mkrom_arguments_parser_path(
      parameter,
      parameter_characters,
      &arguments->path_prefix,
      &arguments->path_prefix_characters
   );
}

static enum Tux64ArgumentsParseStatus
tux64_mkrom_arguments_parser_help(
   const char * parameter,
   Tux64UInt32 parameter_characters,
   void * context
) {
   struct Tux64MkromArguments * arguments;

   arguments = (struct Tux64MkromArguments *)context;

   if (parameter_characters != TUX64_LITERAL_UINT32(0u)) {
      return TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
   }

   (void)arguments;
   (void)parameter;
   tux64_mkrom_arguments_print_menu_help();
   return TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
}

static enum Tux64ArgumentsParseStatus
tux64_mkrom_arguments_parser_version(
   const char * parameter,
   Tux64UInt32 parameter_characters,
   void * context
) {
   struct Tux64MkromArguments * arguments;

   arguments = (struct Tux64MkromArguments *)context;

   if (parameter_characters != TUX64_LITERAL_UINT32(0u)) {
      return TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
   }

   (void)arguments;
   (void)parameter;
   tux64_mkrom_arguments_print_menu_version();
   return TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
}

static const char *
tux64_mkrom_arguments_option_config_identifiers_long [] = {
   "config"
};
static const char
tux64_mkrom_arguments_option_config_identifiers_short [] = {
   'c'
};
static const char *
tux64_mkrom_arguments_option_output_identifiers_long [] = {
   "output"
};
static const char
tux64_mkrom_arguments_option_output_identifiers_short [] = {
   'o'
};
static const char *
tux64_mkrom_arguments_option_prefix_identifiers_long [] = {
   "prefix"
};
static const char
tux64_mkrom_arguments_option_prefix_identifiers_short [] = {
   'p'
};
static const char *
tux64_mkrom_arguments_option_help_identifiers_long [] = {
   "help"
};
static const char
tux64_mkrom_arguments_option_help_identifiers_short [] = {
   'h', '?'
};
static const char *
tux64_mkrom_arguments_option_version_identifiers_long [] = {
   "version"
};
static const char
tux64_mkrom_arguments_option_version_identifiers_short [] = {
   'v'
};

#define TUX64_MKROM_ARGUMENTS_OPTION_CONFIG_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_config_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_OPTION_CONFIG_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_config_identifiers_short)
#define TUX64_MKROM_ARGUMENTS_OPTION_OUTPUT_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_output_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_OPTION_OUTPUT_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_output_identifiers_short)
#define TUX64_MKROM_ARGUMENTS_OPTION_PREFIX_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_prefix_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_OPTION_PREFIX_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_prefix_identifiers_short)
#define TUX64_MKROM_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_help_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_help_identifiers_short)
#define TUX64_MKROM_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_version_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_option_version_identifiers_short)

#define TUX64_MKROM_ARGUMENTS_OPTION_PREFIX_DEFAULT_VALUE\
   ""

static const struct Tux64ArgumentsOption
tux64_mkrom_arguments_options_required [] = {
   {
      .identifiers_long          = tux64_mkrom_arguments_option_config_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_option_config_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_CONFIG_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_CONFIG_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_parser_config
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_option_output_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_option_output_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_OUTPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_OUTPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_parser_output
   },
};

static const struct Tux64ArgumentsOption
tux64_mkrom_arguments_options_optional [] = {
   {
      .identifiers_long          = tux64_mkrom_arguments_option_prefix_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_option_prefix_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_PREFIX_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_PREFIX_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_parser_prefix
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_option_help_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_option_help_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_parser_help
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_option_version_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_option_version_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_parser_version
   }
};

#define TUX64_MKROM_ARGUMENTS_LIST_REQUIRED_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_options_required)
#define TUX64_MKROM_ARGUMENTS_LIST_OPTIONAL_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_options_optional)
#define TUX64_MKROM_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES\
   ((TUX64_MKROM_ARGUMENTS_LIST_REQUIRED_COUNT + 8u) / 8u)

#define TUX64_MKROM_ARGUMENTS_PREFIX_LONG\
   "--"
#define TUX64_MKROM_ARGUMENTS_PREFIX_SHORT\
   "-"

#define TUX64_MKROM_ARGUMENTS_PREFIX_LONG_CHARACTERS\
   TUX64_ARRAY_ELEMENTS(TUX64_MKROM_ARGUMENTS_PREFIX_LONG)
#define TUX64_MKROM_ARGUMENTS_PREFIX_SHORT_CHARACTERS\
   TUX64_ARRAY_ELEMENTS(TUX64_MKROM_ARGUMENTS_PREFIX_SHORT)

static const struct Tux64ArgumentsList
tux64_mkrom_arguments_list = {
   .options_required          = tux64_mkrom_arguments_options_required,
   .options_optional          = tux64_mkrom_arguments_options_optional,
   .options_required_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_LIST_REQUIRED_COUNT),
   .options_optional_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_LIST_OPTIONAL_COUNT),
   .prefix_long               = TUX64_MKROM_ARGUMENTS_PREFIX_LONG,
   .prefix_short              = TUX64_MKROM_ARGUMENTS_PREFIX_SHORT,
   .prefix_long_characters    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_PREFIX_LONG_CHARACTERS),
   .prefix_short_characters   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_PREFIX_SHORT_CHARACTERS),
};

static void
tux64_mkrom_arguments_initialize_optional(
   struct Tux64MkromArguments * output
) {
   output->path_prefix = TUX64_MKROM_ARGUMENTS_OPTION_PREFIX_DEFAULT_VALUE;
   return;
}

struct Tux64ArgumentsParseResult
tux64_mkrom_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64MkromArguments * output
) {
   Tux64UInt8 required_storage [TUX64_MKROM_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES];

   tux64_mkrom_arguments_initialize_optional(output);

   return tux64_arguments_parse(
      &tux64_mkrom_arguments_list,
      input,
      output,
      required_storage
   );
}

#define TUX64_MKROM_ARGUMENTS_MENU_HELP\
   TUX64_MKROM_PACKAGE_NAME " - creates a bootable Nintendo 64 ROM image for Tux64\n"\
   "\n"\
   "COMMAND-LINE OPTIONS:\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_PREFIX_SHORT "c, " TUX64_MKROM_ARGUMENTS_PREFIX_LONG "config=[path]\n"\
   "\n"\
   "      The path to read the boot configuration file from.\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_PREFIX_SHORT "o, " TUX64_MKROM_ARGUMENTS_PREFIX_LONG "output=[path]\n"\
   "\n"\
   "      The path to write the Nintendo 64 ROM image to.\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_PREFIX_SHORT "p, " TUX64_MKROM_ARGUMENTS_PREFIX_LONG "prefix=[path], default=\"" TUX64_MKROM_ARGUMENTS_OPTION_PREFIX_DEFAULT_VALUE "\"\n"\
   "\n"\
   "      The path to prepend to all file paths, both on the command-line and in the\n"\
   "      configuration file.\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_PREFIX_SHORT "h, " TUX64_MKROM_ARGUMENTS_PREFIX_SHORT "?, " TUX64_MKROM_ARGUMENTS_PREFIX_LONG "help\n"\
   "\n"\
   "      Prints the program's help menu and usage information.\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_PREFIX_SHORT "v, " TUX64_MKROM_ARGUMENTS_PREFIX_LONG "version\n"\
   "\n"\
   "      Prints the program's name and version.\n"\
   "\n"\
   "CONFIGURATION FILE:\n"\
   "\n"\
   "   Most of the boot parameters required to build the ROM image are stored in a\n"\
   "   simple configuration file in order to cut down on repetitiveness.\n"\
   "   Configuration items follow the following format for each line:\n"\
   "\n"\
   "      [key]=[value]\n"\
   "\n"\
   "   The following are recognized configuration items:\n"\
   "\n"\
   "      bootloader-stage0=[path]\n"\
   "\n"\
   "         The path to the stage-0 tux64-boot bootloader binary.\n"\
   "\n"\
   "      bootloader-stage0-cic=[path]\n"\
   "\n"\
   "         The path to the stage-0 tux64-boot bootloader CIC data.\n"\
   "\n"\
   "      bootloader-stage1=[path]\n"\
   "\n"\
   "         The path to the stage-1 tux64-boot bootloader binary.\n"\
   "\n"\
   "      bootloader-stage2=[path]\n"\
   "\n"\
   "         The path to the stage-2 tux64-boot bootloader binary.\n"\
   "\n"\
   "      bootloader-stage2-bss=[path]\n"\
   "\n"\
   "         The path to the stage-2 tux64-boot bootloader BSS data length file.\n"\
   "\n"\
   "      bootloader-stage3=[path]\n"\
   "\n"\
   "         The path to the stage-3 tux64-boot bootloader binary.\n"\
   "\n"\
   "      kernel=[path]\n"\
   "\n"\
   "         The path to the kernel image.\n"\
   "\n"\
   "      initramfs=[path]\n"\
   "\n"\
   "         The path to the kernel's initramfs.\n"\
   "\n"\
   "      command-line=[text]\n"\
   "\n"\
   "         The kernel command-line to boot the kernel image with.\n"\
   "\n"\
   "EXAMPLE USAGE:\n"\
   "\n"\
   "   " TUX64_MKROM_PACKAGE_NAME " " TUX64_MKROM_ARGUMENTS_PREFIX_LONG "config=tux64-mkrom.cfg " TUX64_MKROM_ARGUMENTS_PREFIX_LONG "output=tux64.n64\n"\
   "\n"

#define TUX64_MKROM_ARGUMENTS_MENU_VERSION\
   TUX64_MKROM_PACKAGE_NAME " version " TUX64_MKROM_PACKAGE_VERSION "\n"\
   "For bug reports, please contact " TUX64_MKROM_PACKAGE_BUGREPORT "\n"

static void
tux64_mkrom_arguments_print_menu(
   const char * text,
   Tux64UInt32 bytes
) {
   (void)fwrite(text, (size_t)bytes, (size_t)1u, stderr);
   return;
}

void
tux64_mkrom_arguments_print_menu_help(void) {
   tux64_mkrom_arguments_print_menu(
      TUX64_MKROM_ARGUMENTS_MENU_HELP,
      sizeof(TUX64_MKROM_ARGUMENTS_MENU_HELP)
   );
   return;   
}

void
tux64_mkrom_arguments_print_menu_version(void) {
   tux64_mkrom_arguments_print_menu(
      TUX64_MKROM_ARGUMENTS_MENU_VERSION,
      sizeof(TUX64_MKROM_ARGUMENTS_MENU_VERSION)
   );
   return;
}

