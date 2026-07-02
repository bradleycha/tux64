/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-sramdumper/arguments.c - Implementations for arguments     */
/*    parsing.                                                                */
/*----------------------------------------------------------------------------*/

#include "tux64-sramdumper/tux64-sramdumper.h"
#include "tux64-sramdumper/arguments.h"

#include <tux64/arguments.h>
#include <stdio.h>

static struct Tux64ArgumentsParseOptionResult
tux64_sramdumper_arguments_parser_string(
   const struct Tux64String * parameter,
   struct Tux64String * entry
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters == TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING;
      return result;
   }

   *entry = *parameter;

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_OK;
   return result;
}

static struct Tux64ArgumentsParseOptionResult
tux64_sramdumper_arguments_parser_path_input(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64SramDumperArguments * arguments;

   arguments = (struct Tux64SramDumperArguments *)context;

   return tux64_sramdumper_arguments_parser_string(
      parameter,
      &arguments->path_input
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_sramdumper_arguments_parser_path_output(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64SramDumperArguments * arguments;

   arguments = (struct Tux64SramDumperArguments *)context;

   return tux64_sramdumper_arguments_parser_string(
      parameter,
      &arguments->path_output
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_sramdumper_arguments_parser_help(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_sramdumper_arguments_print_menu_help();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

static struct Tux64ArgumentsParseOptionResult
tux64_sramdumper_arguments_parser_version(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_sramdumper_arguments_print_menu_version();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG \
   "input"
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_SHORT \
   'i'
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG \
   "output"
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_SHORT \
   'o'
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG \
   "help"
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_A \
   'h'
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_B \
   '?'
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG \
   "version"
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_SHORT \
   'v'

#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG)

static const struct Tux64String
tux64_sramdumper_arguments_option_path_input_identifiers_long [] = {
   {
      .ptr        = TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG_CHARACTERS)
   }
};

static const char
tux64_sramdumper_arguments_option_path_input_identifiers_short [] = {
   TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_SHORT
};

static const struct Tux64String
tux64_sramdumper_arguments_option_path_output_identifiers_long [] = {
   {
      .ptr        = TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG_CHARACTERS)
   }
};

static const char
tux64_sramdumper_arguments_option_path_output_identifiers_short [] = {
   TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_SHORT
};

static const struct Tux64String
tux64_sramdumper_arguments_option_help_identifiers_long [] = {
   {
      .ptr        = TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS)
   }
};

static const char
tux64_sramdumper_arguments_option_help_identifiers_short [] = {
   TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_A,
   TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_B
};

static const struct Tux64String
tux64_sramdumper_arguments_option_version_identifiers_long [] = {
   {
      .ptr        = TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS)
   }
};

static const char
tux64_sramdumper_arguments_option_version_identifiers_short [] = {
   TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_SHORT
};

#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_option_path_input_identifiers_long)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_option_path_input_identifiers_short)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_option_path_output_identifiers_long)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_option_path_output_identifiers_short)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_option_help_identifiers_long)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_option_help_identifiers_short)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_option_version_identifiers_long)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_option_version_identifiers_short)

static const struct Tux64ArgumentsOption
tux64_sramdumper_arguments_options_required [] = {
   {
      .identifiers_long          = tux64_sramdumper_arguments_option_path_input_identifiers_long,
      .identifiers_short         = tux64_sramdumper_arguments_option_path_input_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_sramdumper_arguments_parser_path_input
   },
   {
      .identifiers_long          = tux64_sramdumper_arguments_option_path_output_identifiers_long,
      .identifiers_short         = tux64_sramdumper_arguments_option_path_output_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_sramdumper_arguments_parser_path_output
   }
};

static const struct Tux64ArgumentsOption
tux64_sramdumper_arguments_options_optional [] = {
   {
      .identifiers_long          = tux64_sramdumper_arguments_option_help_identifiers_long,
      .identifiers_short         = tux64_sramdumper_arguments_option_help_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_sramdumper_arguments_parser_help
   },
   {
      .identifiers_long          = tux64_sramdumper_arguments_option_version_identifiers_long,
      .identifiers_short         = tux64_sramdumper_arguments_option_version_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_sramdumper_arguments_parser_version
   }
};

#define TUX64_SRAMDUMPER_ARGUMENTS_OPTIONS_REQUIRED_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_options_required)
#define TUX64_SRAMDUMPER_ARGUMENTS_OPTIONS_OPTIONAL_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_sramdumper_arguments_options_optional)
#define TUX64_SRAMDUMPER_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES \
   ((TUX64_SRAMDUMPER_ARGUMENTS_OPTIONS_REQUIRED_COUNT + 8u) / 8u)

#define TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG \
   "--"
#define TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT \
   "-"

#define TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG)
#define TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT)

#define TUX64_SRAMDUMPER_ARGUMENTS_IDENTIFIER_PARAMETER_SPLIT_TOKEN \
   '='

static const struct Tux64ArgumentsList
tux64_sramdumper_arguments_list = {
   .options_required                = tux64_sramdumper_arguments_options_required,
   .options_optional                = tux64_sramdumper_arguments_options_optional,
   .options_required_count          = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTIONS_REQUIRED_COUNT),
   .options_optional_count          = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_OPTIONS_OPTIONAL_COUNT),
   .prefix_long                     = {
      .ptr        = TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG_CHARACTERS)
   },
   .prefix_short                    = {
      .ptr        = TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT,
      .characters = TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT_CHARACTERS)
   },
   .identifier_parameter_split_token = TUX64_SRAMDUMPER_ARGUMENTS_IDENTIFIER_PARAMETER_SPLIT_TOKEN
};

struct Tux64ArgumentsParseResult
tux64_sramdumper_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64SramDumperArguments * output
) {
   Tux64UInt8 required_storage [TUX64_SRAMDUMPER_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES];

   return tux64_arguments_parse(
      &tux64_sramdumper_arguments_list,
      input,
      output,
      required_storage
   );
}

#define TUX64_SRAMDUMPER_ARGUMENTS_MENU_HELP \
   TUX64_SRAMDUMPER_PACKAGE_NAME " - parse tux64-boot stage-0 SRAM memory dumps\n" \
   "\n" \
   "SYNOPSIS:\n" \
   "\n" \
   "   This program is used to parse SRAM-based status code and register dumps from\n" \
   "   tux64-boot's stage-0 bootloader.  It takes as input an SRAM save dump and\n" \
   "   outputs a human-readable text file version of the SRAM dump.\n" \
   "\n" \
   "COMMAND-LINE OPTIONS:\n" \
   "\n" \
   "   " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT "i, " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG "input=[path]\n" \
   "\n" \
   "      The path to the SRAM save file dump.  While not required, it should be\n" \
   "      32KiB if dumped correctly.  It only needs to be large enough to contain\n" \
   "      the SRAM dump.\n" \
   "\n" \
   "   " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT "o, " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG "output=[path]\n" \
   "\n" \
   "      The path to output the human-readable text file of the SRAM dump.\n" \
   "\n" \
   "   " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT "h, " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT "?, " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG "help\n" \
   "\n" \
   "      Prints the program's help menu and usage information.\n" \
   "\n" \
   "   " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_SHORT "v, " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG "version\n" \
   "\n" \
   "      Prints the program's name and version.\n" \
   "\n" \
   "EXAMPLE USAGE:\n" \
   "\n" \
   "   " TUX64_SRAMDUMPER_PACKAGE_NAME " " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG "input=sram.bin " TUX64_SRAMDUMPER_ARGUMENTS_PREFIX_LONG "output=dump.txt\n" \
   "\n"

#define TUX64_SRAMDUMPER_ARGUMENTS_MENU_VERSION \
   TUX64_SRAMDUMPER_PACKAGE_NAME " version " TUX64_SRAMDUMPER_PACKAGE_VERSION "\n" \
   "For bug reports, please contact " TUX64_SRAMDUMPER_PACKAGE_BUGREPORT "\n"

static void
tux64_sramdumper_arguments_print_menu(
   const char * text,
   Tux64UInt32 bytes
) {
   (void)fwrite(text, (size_t)bytes, (size_t)1u, stdout);
}

void
tux64_sramdumper_arguments_print_menu_help(void) {
   tux64_sramdumper_arguments_print_menu(
      TUX64_SRAMDUMPER_ARGUMENTS_MENU_HELP,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_SRAMDUMPER_ARGUMENTS_MENU_HELP) * sizeof(char))
   );
   return;
}

void
tux64_sramdumper_arguments_print_menu_version(void) {
   tux64_sramdumper_arguments_print_menu(
      TUX64_SRAMDUMPER_ARGUMENTS_MENU_VERSION,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_SRAMDUMPER_ARGUMENTS_MENU_VERSION) * sizeof(char))
   );
   return;
}

