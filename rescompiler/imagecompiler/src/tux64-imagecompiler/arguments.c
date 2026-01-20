/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/imagecompiler/src/tux64-imagecompiler/arguments.c -            */
/*    Implementations for arguments parsing.                                  */
/*----------------------------------------------------------------------------*/

#include "tux64-imagecompiler/tux64-imagecompiler.h"
#include "tux64-imagecompiler/arguments.h"

#include <tux64/arguments.h>
#include <stdio.h>

static struct Tux64ArgumentsParseOptionResult
tux64_imagecompiler_arguments_parser_string(
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
tux64_imagecompiler_arguments_parser_path_input(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ImageCompilerArguments * arguments;

   arguments = (struct Tux64ImageCompilerArguments *)context;

   return tux64_imagecompiler_arguments_parser_string(
      parameter,
      &arguments->path_input
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_imagecompiler_arguments_parser_path_output(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ImageCompilerArguments * arguments;

   arguments = (struct Tux64ImageCompilerArguments *)context;

   return tux64_imagecompiler_arguments_parser_string(
      parameter,
      &arguments->path_output
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_imagecompiler_arguments_parser_name_pixels(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ImageCompilerArguments * arguments;

   arguments = (struct Tux64ImageCompilerArguments *)context;

   return tux64_imagecompiler_arguments_parser_string(
      parameter,
      &arguments->name_pixels
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_imagecompiler_arguments_parser_name_color_table(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ImageCompilerArguments * arguments;

   arguments = (struct Tux64ImageCompilerArguments *)context;

   return tux64_imagecompiler_arguments_parser_string(
      parameter,
      &arguments->name_color_table
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_imagecompiler_arguments_parser_help(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_imagecompiler_arguments_print_menu_help();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

static struct Tux64ArgumentsParseOptionResult
tux64_imagecompiler_arguments_parser_version(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_imagecompiler_arguments_print_menu_version();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG \
   "input"
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_SHORT \
   'i'
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG \
   "output"
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_SHORT \
   'o'
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_PIXELS_IDENTIFIER \
   "name-pixels"
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_COLOR_TABLE_IDENTIFIER \
   "name-color-table"
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG \
   "help"
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_A \
   'h'
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_B \
   '?'
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG \
   "version"
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_SHORT \
   'v'

#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_PIXELS_IDENTIFIER_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_PIXELS_IDENTIFIER)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_COLOR_TABLE_IDENTIFIER_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_COLOR_TABLE_IDENTIFIER)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG)

static const struct Tux64String
tux64_imagecompiler_arguments_option_path_input_identifiers_long [] = {
   {
      .ptr        = TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_LONG_CHARACTERS)
   }
};

static const char
tux64_imagecompiler_arguments_option_path_input_identifiers_short [] = {
   TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIER_SHORT
};

static const struct Tux64String
tux64_imagecompiler_arguments_option_path_output_identifiers_long [] = {
   {
      .ptr        = TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_LONG_CHARACTERS)
   }
};

static const char
tux64_imagecompiler_arguments_option_path_output_identifiers_short [] = {
   TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIER_SHORT
};

static const struct Tux64String
tux64_imagecompiler_arguments_option_name_pixels_identifiers [] = {
   {
      .ptr        = TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_PIXELS_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_PIXELS_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_imagecompiler_arguments_option_name_color_table_identifiers [] = {
   {
      .ptr        = TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_COLOR_TABLE_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_COLOR_TABLE_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_imagecompiler_arguments_option_help_identifiers_long [] = {
   {
      .ptr        = TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS)
   }
};

static const char
tux64_imagecompiler_arguments_option_help_identifiers_short [] = {
   TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_A,
   TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIER_SHORT_B
};

static const struct Tux64String
tux64_imagecompiler_arguments_option_version_identifiers_long [] = {
   {
      .ptr        = TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS)
   }
};

static const char
tux64_imagecompiler_arguments_option_version_identifiers_short [] = {
   TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIER_SHORT
};

#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_path_input_identifiers_long)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_path_input_identifiers_short)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_path_output_identifiers_long)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_path_output_identifiers_short)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_PIXELS_IDENTIFIERS_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_name_pixels_identifiers)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_COLOR_TABLE_IDENTIFIERS_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_name_color_table_identifiers)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_help_identifiers_long)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_help_identifiers_short)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_version_identifiers_long)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_option_version_identifiers_short)

static const struct Tux64ArgumentsOption
tux64_imagecompiler_arguments_options_required [] = {
   {
      .identifiers_long          = tux64_imagecompiler_arguments_option_path_input_identifiers_long,
      .identifiers_short         = tux64_imagecompiler_arguments_option_path_input_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_INPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_imagecompiler_arguments_parser_path_input
   },
   {
      .identifiers_long          = tux64_imagecompiler_arguments_option_path_output_identifiers_long,
      .identifiers_short         = tux64_imagecompiler_arguments_option_path_output_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_PATH_OUTPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_imagecompiler_arguments_parser_path_output
   },
   {
      .identifiers_long          = tux64_imagecompiler_arguments_option_name_pixels_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_PIXELS_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_imagecompiler_arguments_parser_name_pixels
   },
   {
      .identifiers_long          = tux64_imagecompiler_arguments_option_name_color_table_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_NAME_COLOR_TABLE_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_imagecompiler_arguments_parser_name_color_table
   }
};

static const struct Tux64ArgumentsOption
tux64_imagecompiler_arguments_options_optional [] = {
   {
      .identifiers_long          = tux64_imagecompiler_arguments_option_help_identifiers_long,
      .identifiers_short         = tux64_imagecompiler_arguments_option_help_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_HELP_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_imagecompiler_arguments_parser_help
   },
   {
      .identifiers_long          = tux64_imagecompiler_arguments_option_version_identifiers_long,
      .identifiers_short         = tux64_imagecompiler_arguments_option_version_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_imagecompiler_arguments_parser_version
   }
};

#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTIONS_REQUIRED_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_options_required)
#define TUX64_IMAGECOMPILER_ARGUMENTS_OPTIONS_OPTIONAL_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_imagecompiler_arguments_options_optional)
#define TUX64_IMAGECOMPILER_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES \
   ((TUX64_IMAGECOMPILER_ARGUMENTS_OPTIONS_REQUIRED_COUNT + 8u) / 8u)

#define TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG \
   "--"
#define TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT \
   "-"

#define TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG)
#define TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT_CHARACTERS \
   TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT)

#define TUX64_IMAGECOMPILER_ARGUMENTS_IDENTIFIER_PARAMETER_SPLIT_TOKEN \
   '='

static const struct Tux64ArgumentsList
tux64_imagecompiler_arguments_list = {
   .options_required                   = tux64_imagecompiler_arguments_options_required,
   .options_optional                   = tux64_imagecompiler_arguments_options_optional,
   .options_required_count             = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTIONS_REQUIRED_COUNT),
   .options_optional_count             = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_OPTIONS_OPTIONAL_COUNT),
   .prefix_long                        = {
      .ptr        = TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG_CHARACTERS),
   },
   .prefix_short                       = {
      .ptr        = TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT,
      .characters = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT_CHARACTERS),
   },
   .identifier_parameter_split_token   = TUX64_IMAGECOMPILER_ARGUMENTS_IDENTIFIER_PARAMETER_SPLIT_TOKEN
};

struct Tux64ArgumentsParseResult
tux64_imagecompiler_arguments_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64ImageCompilerArguments * output
) {
   Tux64UInt8 required_storage [TUX64_IMAGECOMPILER_ARGUMENTS_LIST_REQUIRED_STORAGE_BYTES];

   return tux64_arguments_parse(
      &tux64_imagecompiler_arguments_list,
      input,
      output,
      required_storage
   );
}

#define TUX64_IMAGECOMPILER_ARGUMENTS_MENU_HELP \
   TUX64_IMAGECOMPILER_PACKAGE_NAME " - converts a bitmap image into a compressed format formatted as C source code\n" \
   "\n" \
   "SYNOPSIS:\n" \
   "\n" \
   "   This program is used by tux64-boot to embed image data to render as the logo.\n" \
   "   The image should be 64x64 pixels, have no more than 15 unique colors, and\n" \
   "   have a 1-bit alpha layer.\n" \
   "\n" \
   "   The image data is first encoded as color-indexed, with 4 bits per pixel.\n" \
   "   Color index zero is reserved to be transparent, with 15 possible colors.\n" \
   "   The color table which encodes the color for each index encodes all colors as\n" \
   "   big-endian RGBA5551.\n" \
   "\n" \
   "   Once encoded to CI4/RGBA5551, the image data is compressed.  The\n" \
   "   compression algorithm is a modified version of run-length encoding (RLE),\n" \
   "   where compression can be enabled and disabled on the fly, which allows both\n" \
   "   decent compression for long pixel runs and almost zero overhead for very\n" \
   "   short pixel runs.  In addition, pixels are grouped together in pairs of two,\n" \
   "   referred to as pixel tuples.  This allows excellent compression of dither\n" \
   "   patterns.\n" \
   "\n" \
   "   The encoded and compressed image data is then formatted as C source code.\n" \
   "   The formatted source code is two preprocessor defines, one for the pixel\n" \
   "   data, and the other for the color table data.  These defines can be used to\n" \
   "   statically initialize an array which embeds the pixel and color table data.\n" \
   "\n" \
   "   For example, if we have the pixel data name\n" \
   "   \'TUX64_BOOT_STAGE1_LOGO_PIXEL_DATA\' and color table name\n" \
   "   \'TUX64_BOOT_STAGE1_LOGO_COLOR_TABLE_DATA\', the generated C source code\n" \
   "   will look similar to the following:\n" \
   "\n" \
   "      #define TUX64_BOOT_STAGE1_LOGO_PIXEL_DATA \\\n" \
   "         TUX64_LITERAL_UINT8(0x00), \\\n" \
   "         TUX64_LITERAL_UINT8(0x01), \\\n" \
   "         ...\n" \
   "         TUX64_LITERAL_UINT8(0xff)\n" \
   "\n" \
   "      #define TUX64_BOOT_STAGE1_LOGO_COLOR_TABLE_DATA \\\n" \
   "         TUX64_LITERAL_UINT16(0x0001), \\\n" \
   "         TUX64_LITERAL_UINT16(0xffff), \\\n" \
   "         ...\n" \
   "         TUX64_LITERAL_UINT16(0x0123)\n" \
   "\n" \
   "   This source code should be generated by the build system and included with\n" \
   "   a #include \"...\" directive in the file which uses it.  You can then use\n" \
   "   this to store the data however you like.  For example, you can do the\n" \
   "   following:\n" \
   "\n" \
   "      #include <tux64/tux64.h>\n" \
   "      #include \"logo.bin.c\"\n" \
   "\n" \
   "      const Tux64UInt8\n" \
   "      tux64_boot_stage1_logo_pixel_data [] = {\n" \
   "         TUX64_BOOT_STAGE1_LOGO_PIXEL_DATA\n" \
   "      };\n" \
   "\n" \
   "      const Tux64UInt16\n" \
   "      tux64_boot_stage1_logo_color_table_data [] = {\n" \
   "         TUX64_BOOT_STAGE1_LOGO_COLOR_TABLE_DATA\n" \
   "      };\n" \
   "\n" \
   "COMMAND-LINE OPTIONS:\n" \
   "\n" \
   "   " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT "i, " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "input=[path]\n" \
   "\n" \
   "      The path to the input raw image data.  This should be a raw 64x64 RGBA8888\n" \
   "      image with no header or compression.  This kind of file can be exported\n" \
   "      from GIMP with Export As > Raw image data (data,raw) with the Planar\n" \
   "      configuration set to \"Contiguous\" and Palette\'s layout set to\n" \
   "      \"R, G, B (normal)\".  Also make sure an alpha layer is present, otherwise an\n" \
   "      alpha channel won\'t be included in the raw data.  This can be done with\n" \
   "      Layer > Transparency > Add Alpha Channel.\n" \
   "\n" \
   "   " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT "o, " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "output=[path]\n" \
   "\n" \
   "      The path to the output C source code.\n" \
   "\n" \
   "   " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "name-pixels=[path]\n" \
   "\n" \
   "      The name of the pixel data in the generated C source code.\n" \
   "\n" \
   "   " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "name-color-table=[path]\n" \
   "\n" \
   "      The name of the color table data in the generated C source code.\n" \
   "\n" \
   "   " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT "h, " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT "?, " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "help\n" \
   "\n" \
   "      Prints the program's help menu and usage information.\n" \
   "\n" \
   "   " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_SHORT "v, " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "version\n" \
   "\n" \
   "      Prints the program's name and version.\n" \
   "\n" \
   "EXAMPLE USAGE:\n" \
   "\n" \
   "   " TUX64_IMAGECOMPILER_PACKAGE_NAME " " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "input=logo.data " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "output=logo.bin.c " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "name-pixels=TUX64_BOOT_STAGE1_LOGO_PIXEL_DATA " TUX64_IMAGECOMPILER_ARGUMENTS_PREFIX_LONG "name-color-table=TUX64_BOOT_STAGE1_LOGO_COLOR_TABLE_DATA\n" \
   "\n" \

#define TUX64_IMAGECOMPILER_ARGUMENTS_MENU_VERSION \
   TUX64_IMAGECOMPILER_PACKAGE_NAME " version " TUX64_IMAGECOMPILER_PACKAGE_VERSION "\n" \
   "For bug reports, please contact " TUX64_IMAGECOMPILER_PACKAGE_BUGREPORT "\n"

static void
tux64_imagecompiler_arguments_print_menu(
   const char * text,
   Tux64UInt32 bytes
) {
   (void)fwrite(text, (size_t)bytes, (size_t)1u, stdout);
   return;
}

void
tux64_imagecompiler_arguments_print_menu_help(void) {
   tux64_imagecompiler_arguments_print_menu(
      TUX64_IMAGECOMPILER_ARGUMENTS_MENU_HELP,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_MENU_HELP) * sizeof(char))
   );
   return;
}

void
tux64_imagecompiler_arguments_print_menu_version(void) {
   tux64_imagecompiler_arguments_print_menu(
      TUX64_IMAGECOMPILER_ARGUMENTS_MENU_VERSION,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_IMAGECOMPILER_ARGUMENTS_MENU_VERSION) * sizeof(char))
   );
   return;
}

