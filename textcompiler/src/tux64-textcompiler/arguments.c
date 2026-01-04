/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* textcompiler/src/tux64-textcompiler/arguments.c - Implementations for      */
/*    arguments parsing.                                                      */
/*----------------------------------------------------------------------------*/

#include "tux64-textcompiler/tux64-textcompiler.h"
#include "tux64-textcompiler/arguments.h"

#include <tux64/arguments.h>
#include <stdio.h>

static struct Tux64ArgumentsParseOptionResult
tux64_textcompiler_arguments_parser_string(
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
tux64_textcompiler_arguments_parser_path_input(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64TextCompilerArguments * arguments;

   arguments = (struct Tux64TextCompilerArguments *)context;

   return tux64_textcompiler_arguments_parser_string(
      parameter,
      &arguments->path_input
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_textcompiler_arguments_parser_path_output(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64TextCompilerArguments * arguments;

   arguments = (struct Tux64TextCompilerArguments *)context;

   return tux64_textcompiler_arguments_parser_string(
      parameter,
      &arguments->path_output
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_textcompiler_arguments_parser_help(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_textcompiler_arguments_print_menu_help();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

static struct Tux64ArgumentsParseOptionResult
tux64_textcompiler_arguments_parser_version(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_textcompiler_arguments_print_menu_version();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG \
   "input"
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_SHORT \
   'i'
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG \
   "output"
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_SHORT \
   'o'
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG \
   "help"
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_A \
   'h'
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_B \
   '?'
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG \
   "version"
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_SHORT \
   'v'

#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG)

static const struct Tux64String
tux64_textcompiler_arguments_option_path_input_identifiers_long [] = {
   {
      .ptr        = TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_textcompiler_arguments_option_path_input_identifiers_short [] = {
   TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_SHORT
};
static const struct Tux64String
tux64_textcompiler_arguments_option_path_output_identifiers_long [] = {
   {
      .ptr        = TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_textcompiler_arguments_option_path_output_identifiers_short [] = {
   TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_SHORT
};
static const struct Tux64String
tux64_textcompiler_arguments_option_help_identifiers_long [] = {
   {
      .ptr        = TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_textcompiler_arguments_option_help_identifiers_short [] = {
   TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_A,
   TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_B
};
static const struct Tux64String
tux64_textcompiler_arguments_option_version_identifiers_long [] = {
   {
      .ptr        = TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_textcompiler_arguments_option_version_identifiers_short [] = {
   TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_SHORT
};

#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_option_path_input_identifiers_long)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_option_path_input_identifiers_short)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_option_path_output_identifiers_long)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_option_path_output_identifiers_short)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_option_help_identifiers_long)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_option_help_identifiers_short)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_option_version_identifiers_long)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_option_version_identifiers_short)

static const struct Tux64ArgumentsOption
tux64_textcompiler_arguments_options_required [] = {
   {
      .identifiers_long          = tux64_textcompiler_arguments_option_path_input_identifiers_long,
      .identifiers_short         = tux64_textcompiler_arguments_option_path_input_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_textcompiler_arguments_parser_path_input
   },
   {
      .identifiers_long          = tux64_textcompiler_arguments_option_path_output_identifiers_long,
      .identifiers_short         = tux64_textcompiler_arguments_option_path_output_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_textcompiler_arguments_parser_path_output
   }
};

static const struct Tux64ArgumentsOption
tux64_textcompiler_arguments_options_optional [] = {
   {
      .identifiers_long          = tux64_textcompiler_arguments_option_help_identifiers_long,
      .identifiers_short         = tux64_textcompiler_arguments_option_help_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_textcompiler_arguments_parser_help
   },
   {
      .identifiers_long          = tux64_textcompiler_arguments_option_version_identifiers_long,
      .identifiers_short         = tux64_textcompiler_arguments_option_version_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_textcompiler_arguments_parser_version
   }
};

#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTIONS_REQUIRED_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_options_required)
#define TUX64_TEXTCOMPILER_ARGUMENTS_OPTIONS_OPTIONAL_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_textcompiler_arguments_options_optional)
#define TUX64_TEXTCOMPILER_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES \
   ((TUX64_TEXTCOMPILER_ARGUMENTS_OPTIONS_REQUIRED_COUNT + 8u) / 8u)

#define TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG \
   "--"
#define TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT \
   "-"

#define TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG)
#define TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT)

#define TUX64_TEXTCOMPILER_ARGUMENTS_IDENTIFIER_PARAMETER_SPLIT_TOKEN \
   '='

static const struct Tux64ArgumentsList
tux64_textcompiler_arguments_list = {
   .options_required                   = tux64_textcompiler_arguments_options_required,
   .options_optional                   = tux64_textcompiler_arguments_options_optional,
   .options_required_count             = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTIONS_REQUIRED_COUNT),
   .options_optional_count             = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_OPTIONS_OPTIONAL_COUNT),
   .prefix_long                        = {
      .ptr        = TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG_CHARACTERS)
   },
   .prefix_short                       = {
      .ptr        = TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT,
      .characters = TUX64_LITERAL_UINT32(TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT_CHARACTERS)
   },
   .identifier_parameter_split_token   = TUX64_TEXTCOMPILER_ARGUMENTS_IDENTIFIER_PARAMETER_SPLIT_TOKEN
};

struct Tux64ArgumentsParseResult
tux64_textcompiler_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64TextCompilerArguments * output
) {
   Tux64UInt8 required_storage [TUX64_TEXTCOMPILER_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES];

   return tux64_arguments_parse(
      &tux64_textcompiler_arguments_list,
      input,
      output,
      required_storage
   );
}

#define TUX64_TEXTCOMPILER_ARGUMENTS_MENU_HELP \
   TUX64_TEXTCOMPILER_PACKAGE_NAME " - converts ASCII text to the embedded form for use with tux64-boot\n" \
   "\n" \
   "SYNOPSIS:\n" \
   "\n" \
   "   This program is used by tux64-boot to encode ASCII text into the embedded form\n" \
   "   used in the stage-1 bootloader's framebuffer console.  It takes as input a\n" \
   "   file containing all strings and identifiers, and outputs C source code which\n" \
   "   defines all the data required for each text string.\n" \
   "\n" \
   "COMMAND-LINE OPTIONS:\n" \
   "\n" \
   "   " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT "i, " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG "input=[path]\n" \
   "\n" \
   "      The path to the text file which specifies the text strings to encode.  For\n" \
   "      syntax, refer to the section TEXT FILE SYNTAX.\n" \
   "\n" \
   "   " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT "o, " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG "output=[path]\n" \
   "\n" \
   "      The path to the output C source code.\n" \
   "\n" \
   "   " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT "h, " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT "?, " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG "help\n" \
   "\n" \
   "      Prints the program's help menu and usage information.\n" \
   "\n" \
   "   " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_SHORT "v, " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG "version\n" \
   "\n" \
   "      Prints the program's name and version.\n" \
   "\n" \
   "TEXT FILE SYNTAX:\n" \
   "\n" \
   "   The text file follows a C-like syntax.  It is designed to look like a series\n" \
   "   of in-place struct initializations.  Text strings are a series of fields\n" \
   "   surrounded by curly braces.  A text file must contain at least one text\n" \
   "   string entry.  An example text file would look like the following:\n" \
   "\n" \
   "      {\n" \
   "         /* here is the first entry into our string table! */\n" \
   "         .identifier_data    = HELLO_WORLD_DATA;\n" \
   "         .identifier_length  = HELLO_WORLD_LENGTH;\n" \
   "         .text               = \"Hello world\";\n" \
   "      }\n" \
   "      {\n" \
   "         /* here is another entry as well! */\n" \
   "         .identifier_data    = FORMATTED_DATA;\n" \
   "         .identifier_length  = FORMATTED_LENGTH;\n" \
   "         .text               = \"w a t...\";\n" \
   "      }\n" \
   "\n" \
   "   Notice how we can include C89-style block comments.  Comments can be placed\n" \
   "   anywhere around a field for a text string, but not in the middle of a field.\n" \
   "\n" \
   "   Here is an in-depth explanation of each field for a text string:\n" \
   "\n" \
   "      .identifier_data=[label]\n" \
   "\n" \
   "         The identifier for a preprocessor #define which includes the byte data\n" \
   "         for the string.  This byte data can be used to define an array as\n" \
   "         follows, given an identifier of \"HELLO_WORLD_DATA\":\n" \
   "\n" \
   "            static const Tux64UInt8\n" \
   "            hello_world_data [] = {\n" \
   "               HELLO_WORLD_DATA\n" \
   "            };\n" \
   "\n" \
   "      .identifier_length=[label]\n" \
   "\n" \
   "         The identifier for a preprocessor #define which defines an integer\n" \
   "         which represents the length of the text string, in characters.\n" \
   "\n" \
   "      .text=[string]\n" \
   "\n" \
   "         The text string to encode.  This must not be empty, and can only\n" \
   "         include any of the following characters:\n" \
   "\n" \
   "            [space character]\n" \
   "            A-Z\n" \
   "            a-z\n" \
   "            0-9\n" \
   "            %\n" \
   "            .\n" \
   "\n" \
   "         In addition, there is a hard upper limit of 255 characters.  This may\n" \
   "         still allow string to overflow, depending on the size of the font map.\n" \
   "\n" \
   "EXAMPLE USAGE:\n" \
   "\n" \
   "   " TUX64_TEXTCOMPILER_PACKAGE_NAME " " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG "input=strings.txt " TUX64_TEXTCOMPILER_ARGUMENTS_PREFIX_LONG "output=strings.bin.c\n" \
   "\n"

#define TUX64_TEXTCOMPILER_ARGUMENTS_MENU_VERSION \
   TUX64_TEXTCOMPILER_PACKAGE_NAME " version " TUX64_TEXTCOMPILER_PACKAGE_VERSION "\n" \
   "For bug reports, please contact " TUX64_TEXTCOMPILER_PACKAGE_BUGREPORT "\n"

static void
tux64_textcompiler_arguments_print_menu(
   const char * text,
   Tux64UInt32 bytes
) {
   (void)fwrite(text, (size_t)bytes, (size_t)1u, stdout);
   return;
}

void
tux64_textcompiler_arguments_print_menu_help(void) {
   tux64_textcompiler_arguments_print_menu(
      TUX64_TEXTCOMPILER_ARGUMENTS_MENU_HELP,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_TEXTCOMPILER_ARGUMENTS_MENU_HELP) * sizeof(char))
   );
   return;
}

void
tux64_textcompiler_arguments_print_menu_version(void) {
   tux64_textcompiler_arguments_print_menu(
      TUX64_TEXTCOMPILER_ARGUMENTS_MENU_VERSION,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_TEXTCOMPILER_ARGUMENTS_MENU_VERSION) * sizeof(char))
   );
   return;
}

