/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* fontcompiler/src/tux64-fontcompiler/arguments.c - Implementations for      */
/*    arguments parsing.                                                      */
/*----------------------------------------------------------------------------*/

#include "tux64-fontcompiler/tux64-fontcompiler.h"
#include "tux64-fontcompiler/arguments.h"

#include <tux64/arguments.h>
#include <stdio.h>

static struct Tux64ArgumentsParseOptionResult
tux64_fontcompiler_arguments_parser_string(
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
tux64_fontcompiler_arguments_parser_path_input(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64FontCompilerArguments * arguments;

   arguments = (struct Tux64FontCompilerArguments *)context;

   return tux64_fontcompiler_arguments_parser_string(
      parameter,
      &arguments->path_input
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_fontcompiler_arguments_parser_path_output(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64FontCompilerArguments * arguments;

   arguments = (struct Tux64FontCompilerArguments *)context;

   return tux64_fontcompiler_arguments_parser_string(
      parameter,
      &arguments->path_output
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_fontcompiler_arguments_parser_name(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64FontCompilerArguments * arguments;

   arguments = (struct Tux64FontCompilerArguments *)context;

   return tux64_fontcompiler_arguments_parser_string(
      parameter,
      &arguments->name
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_fontcompiler_arguments_parser_help(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_fontcompiler_arguments_print_menu_help();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

static struct Tux64ArgumentsParseOptionResult
tux64_fontcompiler_arguments_parser_version(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_fontcompiler_arguments_print_menu_version();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG \
   "input"
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_SHORT \
   'i'
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG \
   "output"
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_SHORT \
   'o'
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIER_LONG \
   "name"
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIER_SHORT \
   'n'
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG \
   "help"
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_A \
   'h'
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_B \
   '?'
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG \
   "version"
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_SHORT \
   'v'

#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIER_LONG)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG)

static const struct Tux64String
tux64_fontcompiler_arguments_option_path_input_identifiers_long [] = {
   {
      .ptr        = TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_fontcompiler_arguments_option_path_input_identifiers_short [] = {
   TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_SHORT
};
static const struct Tux64String
tux64_fontcompiler_arguments_option_path_output_identifiers_long [] = {
   {
      .ptr        = TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_fontcompiler_arguments_option_path_output_identifiers_short [] = {
   TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_SHORT
};
static const struct Tux64String
tux64_fontcompiler_arguments_option_name_identifiers_long [] = {
   {
      .ptr        = TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_fontcompiler_arguments_option_name_identifiers_short [] = {
   TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIER_SHORT
};
static const struct Tux64String
tux64_fontcompiler_arguments_option_help_identifiers_long [] = {
   {
      .ptr        = TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_fontcompiler_arguments_option_help_identifiers_short [] = {
   TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_A,
   TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_B
};
static const struct Tux64String
tux64_fontcompiler_arguments_option_version_identifiers_long [] = {
   {
      .ptr        = TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_fontcompiler_arguments_option_version_identifiers_short [] = {
   TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_SHORT
};

#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_path_input_identifiers_long)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_path_input_identifiers_short)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_path_output_identifiers_long)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_path_output_identifiers_short)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_name_identifiers_long)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_name_identifiers_short)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_help_identifiers_long)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_help_identifiers_short)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_version_identifiers_long)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_option_version_identifiers_short)

static const struct Tux64ArgumentsOption
tux64_fontcompiler_arguments_options_required [] = {
   {
      .identifiers_long          = tux64_fontcompiler_arguments_option_path_input_identifiers_long,
      .identifiers_short         = tux64_fontcompiler_arguments_option_path_input_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_fontcompiler_arguments_parser_path_input
   },
   {
      .identifiers_long          = tux64_fontcompiler_arguments_option_path_output_identifiers_long,
      .identifiers_short         = tux64_fontcompiler_arguments_option_path_output_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_fontcompiler_arguments_parser_path_output
   },
   {
      .identifiers_long          = tux64_fontcompiler_arguments_option_name_identifiers_long,
      .identifiers_short         = tux64_fontcompiler_arguments_option_name_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_NAME_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_fontcompiler_arguments_parser_name
   }
};

static const struct Tux64ArgumentsOption
tux64_fontcompiler_arguments_options_optional [] = {
   {
      .identifiers_long          = tux64_fontcompiler_arguments_option_help_identifiers_long,
      .identifiers_short         = tux64_fontcompiler_arguments_option_help_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_fontcompiler_arguments_parser_help
   },
   {
      .identifiers_long          = tux64_fontcompiler_arguments_option_version_identifiers_long,
      .identifiers_short         = tux64_fontcompiler_arguments_option_version_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_fontcompiler_arguments_parser_version
   }
};

#define TUX64_FONTCOMPILER_ARGUMENTS_OPTIONS_REQUIRED_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_options_required)
#define TUX64_FONTCOMPILER_ARGUMENTS_OPTIONS_OPTIONAL_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_fontcompiler_arguments_options_optional)
#define TUX64_FONTCOMPILER_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES \
   ((TUX64_FONTCOMPILER_ARGUMENTS_OPTIONS_REQUIRED_COUNT + 8u) / 8u)

#define TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG \
   "--"
#define TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT \
   "-"

#define TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG)
#define TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT)

#define TUX64_FONTCOMPILER_ARGUMENTS_IDENTIFIER_PARAMETER_SPLIT_TOKEN \
   '='

static const struct Tux64ArgumentsList
tux64_fontcompiler_arguments_list = {
   .options_required                   = tux64_fontcompiler_arguments_options_required,
   .options_optional                   = tux64_fontcompiler_arguments_options_optional,
   .options_required_count             = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTIONS_REQUIRED_COUNT),
   .options_optional_count             = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_OPTIONS_OPTIONAL_COUNT),
   .prefix_long                        = {
      .ptr        = TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG_CHARACTERS)
   },
   .prefix_short                       = {
      .ptr        = TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT,
      .characters = TUX64_LITERAL_UINT32(TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT_CHARACTERS)
   },
   .identifier_parameter_split_token   = TUX64_FONTCOMPILER_ARGUMENTS_IDENTIFIER_PARAMETER_SPLIT_TOKEN
};

struct Tux64ArgumentsParseResult
tux64_fontcompiler_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64FontCompilerArguments * output
) {
   Tux64UInt8 required_storage [TUX64_FONTCOMPILER_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES];

   return tux64_arguments_parse(
      &tux64_fontcompiler_arguments_list,
      input,
      output,
      required_storage
   );
}

#define TUX64_FONTCOMPILER_ARGUMENTS_MENU_HELP \
   TUX64_FONTCOMPILER_PACKAGE_NAME " - converts a font bitmap image to a bit array formatted as C source code\n" \
   "\n" \
   "SYNOPSIS:\n" \
   "   This program is used by tux64-boot to embed a bitmap font into the stage-1\n" \
   "   bootloader.  Each character is 8x4 pixels, with 64 total characters. The\n" \
   "   characters are stored from left to right in the following order:\n " \
   "\n" \
   "      A-Z\n" \
   "      a-z\n" \
   "      0-9\n" \
   "      %\n" \
   "      .\n" \
   "\n" \
   "   So for example, your bitmap font should be ordered to look like the following:\n" \
   "\n" \
   "      ABCDEFGHIJKLMNOP\n" \
   "      QRSTUVWXYZabcdef\n" \
   "      ghijklmnopqrstuv\n" \
   "      wxyz0123456789%.\n" \
   "\n" \
   "   This will be processed to have a linear order.  That is, each character is\n" \
   "   stored as a stream of bits from left to right, top to bottom.  For example,\n" \
   "   if we have the following bitmap character, it will be processed as follows:\n" \
   "\n" \
   "      . # # .  =  0110  =  0x6\n" \
   "      # . . #  =  1001  =  0x9\n" \
   "      # . . #  =  1001  =  0x9\n" \
   "      # . . #  =  1001  =  0x9\n" \
   "      # # # #  =  1111  =  0xf\n" \
   "      # . . #  =  1001  =  0x9\n" \
   "      # . . #  =  1001  =  0x9\n" \
   "      # . . #  =  1001  =  0x9\n" \
   "\n" \
   "                        = 0x6999f999\n" \
   "\n" \
   "   This conversion will be done for all characters.  Then, this bit stream will\n" \
   "   be concatenated as one 256-byte byte array, starting from top-left, going\n" \
   "   first to the end of each horizontal line, then down one vertical line.\n" \
   "\n" \
   "   The output C source code will look like the following, given an identifier\n" \
   "   of \'TUX64_BOOT_STAGE1_FONTMAP_DATA_COMPRESSED\':\n" \
   "\n" \
   "      #define TUX64_BOOT_STAGE1_FONTMAP_DATA_COMPRESSED \\\n" \
   "         TUX64_LITERAL_UINT8(0x69), \\\n" \
   "         TUX64_LITERAL_UINT8(0x99), \\\n" \
   "         TUX64_LITERAL_UINT8(0xf9), \\\n" \
   "         TUX64_LITERAL_UINT8(0x99), \\\n" \
   "         ...\n" \
   "\n" \
   "   This source code should be generated by the build system and included with\n" \
   "   a #include \"...\" directive in the file which uses it.  You can then use\n" \
   "   this to store the data however you like.  For example, you can do the\n" \
   "   following:\n" \
   "\n" \
   "      #include <tux64/tux64.h>\n" \
   "      #include \"fontmap.bin.c\"\n" \
   "\n" \
   "      const Tux64UInt8\n" \
   "      tux64_boot_stage1_fontmap_compressed [] = {\n" \
   "        TUX64_BOOT_STAGE1_FONTMAP_DATA_COMPRESSED\n" \
   "      };\n" \
   "\n" \
   "COMMAND-LINE OPTIONS:\n" \
   "\n" \
   "   " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT "i, " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG "input=[path]\n" \
   "\n" \
   "      The path to the input raw image data.  This should be a raw 64x32 RGB8\n" \
   "      image with no header or compression.  This kind of file can be exported\n" \
   "      from GIMP with Export As > Raw image data (data,raw) with the Planar\n" \
   "      configuration set to \"Contiguous\" and Palette\'s layout set to\n" \
   "      \"R, G, B (normal)\".  Also make sure to remove any Alpha Channel under\n" \
   "      Layer > Transparency > Remove Alpha Channel.  It's also recommended to\n" \
   "      ensure the image is only 2 color, otherwise issues might arise in\n" \
   "      conversion.  This can be done with Colors > Posterize..., then set\n" \
   "      \"Posterize levels\" to 2.\n" \
   "\n" \
   "   " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT "o, " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG "output=[path]\n" \
   "\n" \
   "      The path to the output C source code.\n" \
   "\n" \
   "   " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT "n, " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG "name=[string]\n" \
   "\n" \
   "      The name of the bit array in the generated C source code.\n" \
   "\n" \
   "   " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT "h, " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT "?, " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG "help\n" \
   "\n" \
   "      Prints the program's help menu and usage information.\n" \
   "\n" \
   "   " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_SHORT "v, " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG "version\n" \
   "\n" \
   "      Prints the program's name and version.\n" \
   "\n" \
   "EXAMPLE USAGE:\n" \
   "\n" \
   "   " TUX64_FONTCOMPILER_PACKAGE_NAME " " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG "input=fontmap.data " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG "output=fontmap.bin.c " TUX64_FONTCOMPILER_ARGUMENTS_PREFIX_LONG "name=tux64_boot_stage1_fontmap\n" \
   "\n"

#define TUX64_FONTCOMPILER_ARGUMENTS_MENU_VERSION \
   TUX64_FONTCOMPILER_PACKAGE_NAME " version " TUX64_FONTCOMPILER_PACKAGE_VERSION "\n" \
   "For bug reports, please contact " TUX64_FONTCOMPILER_PACKAGE_BUGREPORT "\n"

static void
tux64_fontcompiler_arguments_print_menu(
   const char * text,
   Tux64UInt32 bytes
) {
   (void)fwrite(text, (size_t)bytes, (size_t)1u, stdout);
   return;
}

void
tux64_fontcompiler_arguments_print_menu_help(void) {
   tux64_fontcompiler_arguments_print_menu(
      TUX64_FONTCOMPILER_ARGUMENTS_MENU_HELP,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_FONTCOMPILER_ARGUMENTS_MENU_HELP) * sizeof(char))
   );
   return;
}

void
tux64_fontcompiler_arguments_print_menu_version(void) {
   tux64_fontcompiler_arguments_print_menu(
      TUX64_FONTCOMPILER_ARGUMENTS_MENU_VERSION,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_FONTCOMPILER_ARGUMENTS_MENU_VERSION) * sizeof(char))
   );
   return;
}

