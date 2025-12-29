/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/arguments.c - Implementations for arguments parsing. */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/arguments.h"

#include <tux64/arguments.h>
#include <tux64/memory.h>
#include <tux64/endian.h>
#include <tux64/bitwise.h>
#include <tux64/parse/string-integer.h>
#include <tux64/platform/mips/n64/rom.h>
#include <tux64/platform/mips/n64/boot.h>
#include <stdio.h>

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_parser_string(
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

#define TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_INVALID_DIGIT\
   "invalid digit"
#define TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_OUT_OF_RANGE\
   "out of range"

#define TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_INVALID_DIGIT_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_INVALID_DIGIT)
#define TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_OUT_OF_RANGE_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_OUT_OF_RANGE)

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_parser_string_integer_hex_format_result(
   const struct Tux64ParseStringIntegerResult * result
) {
   struct Tux64ArgumentsParseOptionResult option_result;

   switch (result->status) {
      case TUX64_PARSE_STRING_INTEGER_STATUS_OK:
         option_result.status = TUX64_ARGUMENTS_PARSE_STATUS_OK;
         return option_result;
      case TUX64_PARSE_STRING_INTEGER_STATUS_INVALID_DIGIT:
         option_result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID;
         option_result.payload.parameter_invalid.reason.ptr = TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_INVALID_DIGIT;
         option_result.payload.parameter_invalid.reason.characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_INVALID_DIGIT_CHARACTERS);
         return option_result;
      case TUX64_PARSE_STRING_INTEGER_STATUS_OUT_OF_RANGE:
         option_result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID;
         option_result.payload.parameter_invalid.reason.ptr = TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_OUT_OF_RANGE;
         option_result.payload.parameter_invalid.reason.characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_PARSER_STRING_INTEGER_HEX_FORMAT_RESULT_OUT_OF_RANGE_CHARACTERS);
         return option_result;
      default:
         TUX64_UNREACHABLE;
   }
   
   TUX64_UNREACHABLE;
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_parser_string_integer_hex_uint32(
   const struct Tux64String * parameter,
   Tux64UInt32 * entry
) {
   struct Tux64ArgumentsParseOptionResult result;
   struct Tux64ParseStringIntegerResult parse_result;

   if (parameter->characters == TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING;
      return result;
   }

   parse_result = tux64_parse_string_integer_hex_uint32(parameter, entry);

   return tux64_mkrom_arguments_parser_string_integer_hex_format_result(
      &parse_result
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_parser_string_integer_hex_uint64(
   const struct Tux64String * parameter,
   Tux64UInt64 * entry
) {
   struct Tux64ArgumentsParseOptionResult result;
   struct Tux64ParseStringIntegerResult parse_result;

   if (parameter->characters == TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING;
      return result;
   }

   parse_result = tux64_parse_string_integer_hex_uint64(parameter, entry);

   return tux64_mkrom_arguments_parser_string_integer_hex_format_result(
      &parse_result
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_command_line_parser_config(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsCommandLine * arguments;

   arguments = (struct Tux64MkromArgumentsCommandLine *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_config
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_command_line_parser_output(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsCommandLine * arguments;

   arguments = (struct Tux64MkromArgumentsCommandLine *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_output
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_command_line_parser_prefix(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsCommandLine * arguments;

   arguments = (struct Tux64MkromArgumentsCommandLine *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_prefix
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_command_line_parser_help(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_mkrom_arguments_command_line_print_menu_help();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_command_line_parser_version(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   (void)context;
   tux64_mkrom_arguments_command_line_print_menu_version();

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_EXIT;
   return result;
}

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIER_LONG\
   "config"
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIER_SHORT\
   'c'
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIER_LONG\
   "output"
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIER_SHORT\
   'o'
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIER_LONG\
   "prefix"
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIER_SHORT\
   'p'
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIER_LONG\
   "help"
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIER_SHORT_A\
   'h'
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIER_SHORT_B\
   '?'
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIER_LONG\
   "version"
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIER_SHORT\
   'v'

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIER_LONG_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIER_LONG)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIER_LONG_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIER_LONG)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIER_LONG_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIER_LONG)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIER_LONG)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIER_LONG)

static const struct Tux64String
tux64_mkrom_arguments_command_line_option_config_identifiers_long [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIER_LONG_CHARACTERS) 
   }
};
static const char
tux64_mkrom_arguments_command_line_option_config_identifiers_short [] = {
   TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIER_SHORT
};
static const struct Tux64String
tux64_mkrom_arguments_command_line_option_output_identifiers_long [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_mkrom_arguments_command_line_option_output_identifiers_short [] = {
   TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIER_SHORT
};
static const struct Tux64String
tux64_mkrom_arguments_command_line_option_prefix_identifiers_long [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_mkrom_arguments_command_line_option_prefix_identifiers_short [] = {
   TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIER_SHORT
};
static const struct Tux64String
tux64_mkrom_arguments_command_line_option_help_identifiers_long [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_mkrom_arguments_command_line_option_help_identifiers_short [] = {
   TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIER_SHORT_A,
   TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIER_SHORT_B
};
static const struct Tux64String
tux64_mkrom_arguments_command_line_option_version_identifiers_long [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIER_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIER_LONG_CHARACTERS)
   }
};
static const char
tux64_mkrom_arguments_command_line_option_version_identifiers_short [] = {
   TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIER_SHORT
};

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_config_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_config_identifiers_short)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_output_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_output_identifiers_short)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_prefix_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_prefix_identifiers_short)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_help_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_help_identifiers_short)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIERS_LONG_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_version_identifiers_long)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_option_version_identifiers_short)

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_DEFAULT_VALUE\
   ""
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_DEFAULT_VALUE_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_DEFAULT_VALUE)

static const struct Tux64ArgumentsOption
tux64_mkrom_arguments_command_line_options_required [] = {
   {
      .identifiers_long          = tux64_mkrom_arguments_command_line_option_config_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_command_line_option_config_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_CONFIG_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_command_line_parser_config
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_command_line_option_output_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_command_line_option_output_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_OUTPUT_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_command_line_parser_output
   },
};

static const struct Tux64ArgumentsOption
tux64_mkrom_arguments_command_line_options_optional [] = {
   {
      .identifiers_long          = tux64_mkrom_arguments_command_line_option_prefix_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_command_line_option_prefix_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_command_line_parser_prefix
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_command_line_option_help_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_command_line_option_help_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_HELP_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_command_line_parser_help
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_command_line_option_version_identifiers_long,
      .identifiers_short         = tux64_mkrom_arguments_command_line_option_version_identifiers_short,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIERS_LONG_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_VERSION_IDENTIFIERS_SHORT_COUNT),
      .parser                    = tux64_mkrom_arguments_command_line_parser_version
   }
};

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_LIST_REQUIRED_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_options_required)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_LIST_OPTIONAL_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_command_line_options_optional)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_LIST_REQUIRED_STORAGE_BYTES\
   ((TUX64_MKROM_ARGUMENTS_COMMAND_LINE_LIST_REQUIRED_COUNT + 8u) / 8u)

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG\
   "--"
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT\
   "-"

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG)
#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT)

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_IDENTIFIER_PARAMETER_SPLIT_TOKEN\
   '='

static const struct Tux64ArgumentsList
tux64_mkrom_arguments_command_line_list = {
   .options_required                   = tux64_mkrom_arguments_command_line_options_required,
   .options_optional                   = tux64_mkrom_arguments_command_line_options_optional,
   .options_required_count             = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_LIST_REQUIRED_COUNT),
   .options_optional_count             = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_LIST_OPTIONAL_COUNT),
   .prefix_long                        = {
      .ptr        = TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG_CHARACTERS)
   },
   .prefix_short                       = {
      .ptr        = TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT_CHARACTERS)
   },
   .identifier_parameter_split_token   = TUX64_MKROM_ARGUMENTS_COMMAND_LINE_IDENTIFIER_PARAMETER_SPLIT_TOKEN
};

static void
tux64_mkrom_arguments_command_line_initialize_optional(
   struct Tux64MkromArgumentsCommandLine * output
) {
   output->path_prefix.ptr          = TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_DEFAULT_VALUE;
   output->path_prefix.characters   = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_DEFAULT_VALUE_CHARACTERS);
   return;
}

struct Tux64ArgumentsParseResult
tux64_mkrom_arguments_command_line_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64MkromArgumentsCommandLine * output
) {
   Tux64UInt8 required_storage [TUX64_MKROM_ARGUMENTS_COMMAND_LINE_LIST_REQUIRED_STORAGE_BYTES];

   tux64_mkrom_arguments_command_line_initialize_optional(output);

   return tux64_arguments_parse(
      &tux64_mkrom_arguments_command_line_list,
      input,
      output,
      required_storage
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_bootloader_stage0(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_bootloader_stage0
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_bootloader_stage0_cic(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_bootloader_stage0_cic
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_bootloader_stage1(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_bootloader_stage1
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_bootloader_stage1_bss(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_bootloader_stage1_bss
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_bootloader_stage2(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_bootloader_stage2
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_kernel(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_kernel
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_initramfs(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->path_initramfs
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_command_line(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   return tux64_mkrom_arguments_parser_string(
      parameter,
      &arguments->command_line
   );
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_no_checksum(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64ArgumentsParseOptionResult result;
   struct Tux64MkromArgumentsConfigFile * arguments;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   if (parameter->characters != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED;
      return result;
   }

   arguments->boot_header_flags = tux64_bitwise_flags_set_uint32(
      arguments->boot_header_flags,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_NO_CHECKSUM)
   );

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_OK;
   return result;
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_rom_header_clock_rate(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;
   struct Tux64ArgumentsParseOptionResult result;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   result = tux64_mkrom_arguments_parser_string_integer_hex_uint32(
      parameter,
      &arguments->rom_header.clock_rate
   );
   if (result.status != TUX64_ARGUMENTS_PARSE_STATUS_OK) {
      return result;
   }
   arguments->rom_header.clock_rate = tux64_endian_convert_uint32(
      arguments->rom_header.clock_rate,
      TUX64_ENDIAN_FORMAT_BIG
   );
   return result;
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_rom_header_boot_address(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;
   struct Tux64ArgumentsParseOptionResult result;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   result = tux64_mkrom_arguments_parser_string_integer_hex_uint32(
      parameter,
      &arguments->rom_header.boot_address
   );
   if (result.status != TUX64_ARGUMENTS_PARSE_STATUS_OK) {
      return result;
   }
   arguments->rom_header.boot_address = tux64_endian_convert_uint32(
      arguments->rom_header.boot_address,
      TUX64_ENDIAN_FORMAT_BIG
   );
   return result;
}

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_rom_header_check_code(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;
   struct Tux64ArgumentsParseOptionResult result;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   result = tux64_mkrom_arguments_parser_string_integer_hex_uint64(
      parameter,
      &arguments->rom_header.check_code
   );
   if (result.status != TUX64_ARGUMENTS_PARSE_STATUS_OK) {
      return result;
   }
   arguments->rom_header.check_code = tux64_endian_convert_uint64(
      arguments->rom_header.check_code,
      TUX64_ENDIAN_FORMAT_BIG
   );
   return result;
}

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_TITLE_ERROR_TOO_LONG\
   "game title must be less than 20 characters"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_TITLE_ERROR_TOO_LONG_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_TITLE_ERROR_TOO_LONG)

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_rom_header_game_title(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;
   struct Tux64ArgumentsParseOptionResult result;
   Tux64UInt8 sentinel_fill;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   if (parameter->characters == TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING;
      return result;
   }

   if (parameter->characters > TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_TITLE_CHARACTERS)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID;
      result.payload.parameter_invalid.reason.ptr = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_TITLE_ERROR_TOO_LONG;
      result.payload.parameter_invalid.reason.characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_TITLE_ERROR_TOO_LONG_CHARACTERS);
      return result;
   }

   tux64_memory_copy(
      arguments->rom_header.game_title,
      parameter->ptr,
      parameter->characters * TUX64_LITERAL_UINT32(sizeof(char))
   );

   sentinel_fill = ' ';
   tux64_memory_fill(
      &arguments->rom_header.game_title[parameter->characters],
      &sentinel_fill,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_TITLE_CHARACTERS) - parameter->characters,
      TUX64_LITERAL_UINT32(sizeof(sentinel_fill))
   );

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_OK;
   return result;
}

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_CODE_INVALID_LENGTH\
   "game code must be exactly 4 characters"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_CODE_INVALID_LENGTH_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_CODE_INVALID_LENGTH)

static struct Tux64ArgumentsParseOptionResult
tux64_mkrom_arguments_config_file_parser_rom_header_game_code(
   const struct Tux64String * parameter,
   void * context
) {
   struct Tux64MkromArgumentsConfigFile * arguments;
   struct Tux64ArgumentsParseOptionResult result;

   arguments = (struct Tux64MkromArgumentsConfigFile *)context;

   if (parameter->characters == TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING;
      return result;
   }

   if (parameter->characters != TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_CODE_CHARACTERS)) {
      result.status = TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID;
      result.payload.parameter_invalid.reason.ptr = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_CODE_INVALID_LENGTH;
      result.payload.parameter_invalid.reason.characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PARSER_ROM_HEADER_GAME_CODE_INVALID_LENGTH_CHARACTERS);
      return result;
   }

   tux64_memory_copy(
      arguments->rom_header.game_code,
      parameter->ptr,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_CODE_CHARACTERS)
   );

   result.status = TUX64_ARGUMENTS_PARSE_STATUS_OK;
   return result;
}

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_IDENTIFIER\
   "bootloader-stage0"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_CIC_IDENTIFIER\
   "bootloader-stage0-cic"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_IDENTIFIER\
   "bootloader-stage1"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_BSS_IDENTIFIER\
   "bootloader-stage1-bss"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE2_IDENTIFIER\
   "bootloader-stage2"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_KERNEL_IDENTIFIER\
   "kernel"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_INITRAMFS_IDENTIFIER\
   "initramfs"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_IDENTIFIER\
   "command-line"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_NO_CHECKSUM_IDENTIFIER\
   "no-checksum"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_IDENTIFIER\
   "rom-header-clock-rate"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_IDENTIFIER\
   "rom-header-boot-address"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_IDENTIFIER\
   "rom-header-check-code"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_IDENTIFIER\
   "rom-header-game-title"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_CODE_IDENTIFIER\
   "rom-header-game-code"

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_CIC_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_CIC_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_BSS_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_BSS_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE2_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE2_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_KERNEL_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_KERNEL_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_INITRAMFS_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_INITRAMFS_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_NO_CHECKSUM_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_NO_CHECKSUM_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_IDENTIFIER)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_CODE_IDENTIFIER_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_CODE_IDENTIFIER)

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_bootloader_stage0_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_bootloader_stage0_cic_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_CIC_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_CIC_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_bootloader_stage1_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_bootloader_stage1_bss_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_BSS_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_BSS_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_bootloader_stage2_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE2_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE2_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_kernel_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_KERNEL_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_KERNEL_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_initramfs_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_INITRAMFS_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_INITRAMFS_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_command_line_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_no_checksum_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_NO_CHECKSUM_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_NO_CHECKSUM_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_rom_header_clock_rate_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_rom_header_boot_address_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_rom_header_check_code_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_rom_header_game_title_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_IDENTIFIER_CHARACTERS)
   }
};

static const struct Tux64String
tux64_mkrom_arguments_config_file_option_rom_header_game_code_identifiers [] = {
   {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_CODE_IDENTIFIER,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_CODE_IDENTIFIER_CHARACTERS)
   }
};

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_bootloader_stage0_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_CIC_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_bootloader_stage0_cic_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_bootloader_stage1_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_BSS_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_bootloader_stage1_bss_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE2_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_bootloader_stage2_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_KERNEL_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_kernel_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_INITRAMFS_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_initramfs_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_command_line_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_NO_CHECKSUM_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_no_checksum_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_rom_header_clock_rate_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_rom_header_boot_address_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_rom_header_check_code_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_rom_header_game_title_identifiers)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_CODE_IDENTIFIERS_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_option_rom_header_game_code_identifiers)

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_DEFAULT_VALUE\
   ""
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_DEFAULT_VALUE_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_DEFAULT_VALUE)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_DEFAULT_VALUE\
   0x80008135
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_DEFAULT_VALUE_STRING\
   "80008135"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_DEFAULT_VALUE\
   0xcafebabe
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_DEFAULT_VALUE_STRING\
   "cafebabe"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_DEFAULT_VALUE\
   0xbaadf00dfee1deadllu
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_DEFAULT_VALUE_STRING\
   "baadf00dfee1dead"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_DEFAULT_VALUE\
   "TUX64 BOOT IMAGE"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_DEFAULT_VALUE_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_DEFAULT_VALUE)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_CODE_DEFAULT_VALUE\
   "NTXE"

static const struct Tux64ArgumentsOption
tux64_mkrom_arguments_config_file_options_required [] = {
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_bootloader_stage0_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_bootloader_stage0
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_bootloader_stage0_cic_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE0_CIC_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_bootloader_stage0_cic
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_bootloader_stage1_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_bootloader_stage1
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_bootloader_stage1_bss_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE1_BSS_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_bootloader_stage1_bss
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_bootloader_stage2_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_BOOTLOADER_STAGE2_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_bootloader_stage2
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_kernel_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_KERNEL_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_kernel
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_initramfs_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_INITRAMFS_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_initramfs
   }
};

static const struct Tux64ArgumentsOption
tux64_mkrom_arguments_config_file_options_optional [] = {
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_command_line_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_command_line
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_no_checksum_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_NO_CHECKSUM_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_no_checksum
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_rom_header_clock_rate_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_rom_header_clock_rate
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_rom_header_boot_address_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_rom_header_boot_address
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_rom_header_check_code_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_rom_header_check_code
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_rom_header_game_title_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_rom_header_game_title
   },
   {
      .identifiers_long          = tux64_mkrom_arguments_config_file_option_rom_header_game_code_identifiers,
      .identifiers_short         = TUX64_NULLPTR,
      .identifiers_long_count    = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_CODE_IDENTIFIERS_COUNT),
      .identifiers_short_count   = TUX64_LITERAL_UINT32(0u),
      .parser                    = tux64_mkrom_arguments_config_file_parser_rom_header_game_code
   }
};

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_LIST_REQUIRED_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_options_required)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_LIST_OPTIONAL_COUNT\
   TUX64_ARRAY_ELEMENTS(tux64_mkrom_arguments_config_file_options_optional)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_LIST_REQUIRED_STORAGE_BYTES\
   ((TUX64_MKROM_ARGUMENTS_CONFIG_FILE_LIST_REQUIRED_COUNT + 8u) / 8u)

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_LONG\
   "--"
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_SHORT\
   "-"

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_LONG_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_LONG)
#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_SHORT_CHARACTERS\
   TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_SHORT)

#define TUX64_MKROM_ARGUMENTS_CONFIG_FILE_IDENTIFIER_PARAMETER_SPLIT_TOKEN\
   '='

static const struct Tux64ArgumentsList
tux64_mkrom_arguments_config_file_list = {
   .options_required                   = tux64_mkrom_arguments_config_file_options_required,
   .options_optional                   = tux64_mkrom_arguments_config_file_options_optional,
   .options_required_count             = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_LIST_REQUIRED_COUNT),
   .options_optional_count             = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_LIST_OPTIONAL_COUNT),
   .prefix_long = {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_LONG,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_LONG_CHARACTERS)
   },
   .prefix_short                       = {
      .ptr        = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_SHORT,
      .characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_PREFIX_SHORT_CHARACTERS)
   },
   .identifier_parameter_split_token   = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_IDENTIFIER_PARAMETER_SPLIT_TOKEN
};

const struct Tux64ArgumentsIteratorOptionsConfigFile
tux64_mkrom_arguments_config_file_iterator_options = {
   .comment_prefix = '#'
};

static void
tux64_mkrom_arguments_config_file_initialize_optional_rom_header(
   struct Tux64PlatformMipsN64RomHeader * header
) {
   Tux64UInt8 sentinel_fill;

   /* this is designed to be written directly to disk/memory, so we store */
   /* everything as big-endian */

   header->pi_dom1_configuration.reserved_00 = TUX64_LITERAL_UINT8(0x80);
   header->pi_dom1_configuration.flags[0]    = TUX64_LITERAL_UINT8(0x37);
   header->pi_dom1_configuration.flags[1]    = TUX64_LITERAL_UINT8(0x12);
   header->pi_dom1_configuration.flags[2]    = TUX64_LITERAL_UINT8(0x40);

   header->clock_rate   = tux64_endian_convert_uint32(TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_DEFAULT_VALUE), TUX64_ENDIAN_FORMAT_BIG);
   header->boot_address = tux64_endian_convert_uint32(TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_DEFAULT_VALUE), TUX64_ENDIAN_FORMAT_BIG);

   header->libultra_version[0] = '\0';
   header->libultra_version[1] = '\0';
   header->libultra_version[2] = 20;
   header->libultra_version[3] = 'L';

   header->check_code = tux64_endian_convert_uint64(TUX64_LITERAL_UINT64(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_DEFAULT_VALUE), TUX64_ENDIAN_FORMAT_BIG);

   sentinel_fill = TUX64_LITERAL_UINT8(0x00);
   tux64_memory_fill(
      header->reserved_18,
      &sentinel_fill,
      TUX64_LITERAL_UINT32(TUX64_ARRAY_ELEMENTS(header->reserved_18)),
      TUX64_LITERAL_UINT32(sizeof(sentinel_fill))
   );

   tux64_memory_copy(
      header->game_title,
      TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_DEFAULT_VALUE,
      TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_DEFAULT_VALUE_CHARACTERS * sizeof(char))
   );
   sentinel_fill = TUX64_LITERAL_UINT8(' ');
   tux64_memory_fill(
      &header->game_title[TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_DEFAULT_VALUE_CHARACTERS],
      &sentinel_fill,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_TITLE_CHARACTERS - TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_DEFAULT_VALUE_CHARACTERS),
      TUX64_LITERAL_UINT32(sizeof(sentinel_fill))
   );

   sentinel_fill = TUX64_LITERAL_UINT8(TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_UNSPECIFIED);
   tux64_memory_fill(
      header->homebrew_info.controller_type,
      &sentinel_fill,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_COUNT),
      TUX64_LITERAL_UINT32(sizeof(sentinel_fill))
   );

   tux64_memory_copy(
      header->homebrew_info.game_id,
      TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_GAME_ID_MAGIC,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_GAME_ID_CHARACTERS * sizeof(char))
   );

   header->homebrew_info.save_type = TUX64_LITERAL_UINT8(
      TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_NONE
   );

   tux64_memory_copy(
      header->game_code,
      TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_CODE_DEFAULT_VALUE,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_CODE_CHARACTERS * sizeof(char))
   );

   header->rom_version = TUX64_LITERAL_UINT8(0x00);

   return;
}

static void
tux64_mkrom_arguments_config_file_initialize_optional(
   struct Tux64MkromArgumentsConfigFile * output
) {
   tux64_mkrom_arguments_config_file_initialize_optional_rom_header(&output->rom_header);

   output->command_line.ptr = TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_DEFAULT_VALUE;
   output->command_line.characters = TUX64_LITERAL_UINT32(TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_DEFAULT_VALUE_CHARACTERS);
   output->boot_header_flags = TUX64_LITERAL_UINT32(0u);

   return;
}

struct Tux64ArgumentsParseResult
tux64_mkrom_arguments_config_file_parse(
   struct Tux64ArgumentsIterator * input,
   struct Tux64MkromArgumentsConfigFile * output
) {
   Tux64UInt8 required_storage [TUX64_MKROM_ARGUMENTS_CONFIG_FILE_LIST_REQUIRED_STORAGE_BYTES];

   tux64_mkrom_arguments_config_file_initialize_optional(output);

   return tux64_arguments_parse(
      &tux64_mkrom_arguments_config_file_list,
      input,
      output,
      required_storage
   );
}

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_MENU_HELP\
   TUX64_MKROM_PACKAGE_NAME " - creates a bootable Nintendo 64 ROM image for Tux64\n"\
   "\n"\
   "COMMAND-LINE OPTIONS:\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT "c, " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG "config=[path]\n"\
   "\n"\
   "      The path to read the boot configuration file from.\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT "o, " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG "output=[path]\n"\
   "\n"\
   "      The path to write the Nintendo 64 ROM image to.\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT "p, " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG "prefix=[path], default=\"" TUX64_MKROM_ARGUMENTS_COMMAND_LINE_OPTION_PREFIX_DEFAULT_VALUE "\"\n"\
   "\n"\
   "      The path to prepend to all file paths in the configuration file.  File\n"\
   "      paths from the command-line will not have the prefix applied.\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT "h, " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT "?, " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG "help\n"\
   "\n"\
   "      Prints the program's help menu and usage information.\n"\
   "\n"\
   "   " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_SHORT "v, " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG "version\n"\
   "\n"\
   "      Prints the program's name and version.\n"\
   "\n"\
   "CONFIGURATION FILE:\n"\
   "\n"\
   "   Most of the boot parameters required to build the ROM image are stored in a\n"\
   "   simple configuration file in order to cut down on repetitiveness.\n"\
   "   Configuration items follow the following format for each line:\n"\
   "\n"\
   "      --[key]=[value]\n"\
   "\n"\
   "   You may also specify with the following format:\n"\
   "\n"\
   "      --[key]\n"\
   "      [value]\n"\
   "\n"\
   "   Whitespace around each key-value pair is ignored.  In addition, a comment\n"\
   "   may be specified with a '#' character. \n"\
   "\n"\
   "   The following are recognized configuration items:\n"\
   "\n"\
   "      --bootloader-stage0=[path]\n"\
   "\n"\
   "         The path to the stage-0 tux64-boot bootloader binary.\n"\
   "\n"\
   "      --bootloader-stage0-cic=[path]\n"\
   "\n"\
   "         The path to the stage-0 tux64-boot bootloader CIC data.\n"\
   "\n"\
   "      --bootloader-stage1=[path]\n"\
   "\n"\
   "         The path to the stage-1 tux64-boot bootloader binary.\n"\
   "\n"\
   "      --bootloader-stage1-bss=[path]\n"\
   "\n"\
   "         The path to the stage-1 tux64-boot bootloader BSS data length file.\n"\
   "\n"\
   "      --bootloader-stage2=[path]\n"\
   "\n"\
   "         The path to the stage-2 tux64-boot bootloader binary.\n"\
   "\n"\
   "      --kernel=[path]\n"\
   "\n"\
   "         The path to the kernel image.\n"\
   "\n"\
   "      --initramfs=[path]\n"\
   "\n"\
   "         The path to the kernel's initramfs.\n"\
   "\n"\
   "      --command-line=[string], default=\"" TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_COMMAND_LINE_DEFAULT_VALUE "\"\n"\
   "\n"\
   "         The kernel command-line to boot the kernel image with.\n"\
   "\n"\
   "      --no-checksum, default is off\n"\
   "\n"\
   "         Except for the boot header itself, disable all checksum verifications for the bootloader\n"\
   "\n"\
   "      --rom-header-clock-rate=[value], default=\"" TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CLOCK_RATE_DEFAULT_VALUE_STRING "\"\n"\
   "\n"\
   "         The clock rate field stored in the ROM header.  This has no effect\n"\
   "         on the actual clock speed of any of the hardware.  This should be a\n"\
   "         32-bit hexadecimal string.\n"\
   "\n"\
   "      --rom-header-boot-address=[value], default=\"" TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_BOOT_ADDRESS_DEFAULT_VALUE_STRING "\"\n"\
   "\n"\
   "         The boot address field stored in the ROM header.  This has no effect\n"\
   "         on the boot process.  This should be a 32-bit hexadecimal string.\n"\
   "\n"\
   "      --rom-header-check-code=[value], default=\"" TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_CHECK_CODE_DEFAULT_VALUE_STRING "\"\n"\
   "\n"\
   "         The check code field stored in the ROM header.  This has no effect on\n"\
   "         the boot process.  This should be a 64-bit hexadecimal string.\n"\
   "\n"\
   "      --rom-header-game-title=[string], default=\"" TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_DEFAULT_VALUE "\"\n"\
   "\n"\
   "         The name of the ROM as stored in the ROM header.  This must be no\n"\
   "         more than 20 characters. \n"\
   "\n"\
   "      --rom-header-game-code=[string], default=\"" TUX64_MKROM_ARGUMENTS_CONFIG_FILE_OPTION_ROM_HEADER_GAME_TITLE_DEFAULT_VALUE "\"\n"\
   "\n"\
   "         The game code field stored in the ROM header.  This must be a 4\n"\
   "         character string. \n"\
   "\n"\
   "EXAMPLE USAGE:\n"\
   "\n"\
   "   " TUX64_MKROM_PACKAGE_NAME " " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG "config=tux64-mkrom.cfg " TUX64_MKROM_ARGUMENTS_COMMAND_LINE_PREFIX_LONG "output=tux64.n64\n"\
   "\n"

#define TUX64_MKROM_ARGUMENTS_COMMAND_LINE_MENU_VERSION\
   TUX64_MKROM_PACKAGE_NAME " version " TUX64_MKROM_PACKAGE_VERSION "\n"\
   "For bug reports, please contact " TUX64_MKROM_PACKAGE_BUGREPORT "\n"

static void
tux64_mkrom_arguments_command_line_print_menu(
   const char * text,
   Tux64UInt32 bytes
) {
   (void)fwrite(text, (size_t)bytes, (size_t)1u, stdout);
   return;
}

void
tux64_mkrom_arguments_command_line_print_menu_help(void) {
   tux64_mkrom_arguments_command_line_print_menu(
      TUX64_MKROM_ARGUMENTS_COMMAND_LINE_MENU_HELP,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_MENU_HELP) * sizeof(char))
   );
   return;   
}

void
tux64_mkrom_arguments_command_line_print_menu_version(void) {
   tux64_mkrom_arguments_command_line_print_menu(
      TUX64_MKROM_ARGUMENTS_COMMAND_LINE_MENU_VERSION,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_MKROM_ARGUMENTS_COMMAND_LINE_MENU_VERSION) * sizeof(char))
   );
   return;
}

