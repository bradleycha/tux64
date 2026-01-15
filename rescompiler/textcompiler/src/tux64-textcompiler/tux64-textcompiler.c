/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/textcompiler/src/tux64-textcompiler/tux64-textcompiler.c -     */
/*    Main application entrypoint for tux64-textcompiler.                     */
/*----------------------------------------------------------------------------*/

#include "tux64-textcompiler/tux64-textcompiler.h"

#include <tux64/log.h>
#include <tux64/arguments.h>
#include <tux64/memory.h>
#include <tux64/fs.h>
#include "tux64-textcompiler/arguments.h"
#include "tux64-textcompiler/lexer.h"
#include "tux64-textcompiler/formatter.h"

#include <stdlib.h>
#include <inttypes.h>

#define TUX64_TEXTCOMPILER_ARGC_MAX TUX64_UINT8_MAX

enum Tux64TextCompilerExitStatus {
   TUX64_TEXTCOMPILER_EXIT_STATUS_OK = 0u,
   TUX64_TEXTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY,
   TUX64_TEXTCOMPILER_EXIT_STATUS_TOO_MANY_ARGUMENTS,
   TUX64_TEXTCOMPILER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR,
   TUX64_TEXTCOMPILER_EXIT_STATUS_FS_ERROR,
   TUX64_TEXTCOMPILER_EXIT_STATUS_LEXER_PARSE_ERROR
};

struct Tux64TextCompilerExitPayloadTooManyArguments {
   int argc_given;
   Tux64UInt8 argc_max;
};

struct Tux64TextCompilerExitPayloadArgumentsParseError {
   struct Tux64ArgumentsParseResult result;
};

struct Tux64TextCompilerExitPayloadFsError {
   struct Tux64FsResult reason;
};

struct Tux64TextCompilerExitPayloadLexerParseError {
   struct Tux64TextCompilerLexerParseResult reason;
   struct Tux64FsLoadedFile file;
};

union Tux64TextCompilerExitPayload {
   struct Tux64TextCompilerExitPayloadTooManyArguments too_many_arguments;
   struct Tux64TextCompilerExitPayloadArgumentsParseError arguments_parse_error;
   struct Tux64TextCompilerExitPayloadFsError fs_error;
   struct Tux64TextCompilerExitPayloadLexerParseError lexer_parse_error;
};

struct Tux64TextCompilerExitResult {
   enum Tux64TextCompilerExitStatus status;
   union Tux64TextCompilerExitPayload payload;
};

static void
tux64_textcompiler_exit_result_display_too_many_arguments(
   const struct Tux64TextCompilerExitPayloadTooManyArguments * self
) {
   TUX64_LOG_ERROR_FMT(
      "unable to handle %d arguments at once, maximum is %" PRIu8,
      self->argc_given,
      self->argc_max
   );
   return;
}

static void
tux64_textcompiler_exit_result_display_arguments_parse_error(
   const struct Tux64TextCompilerExitPayloadArgumentsParseError * self
) {
   /* yes, I copy-pasted this from tux64-mkrom, and now also */
   /* tux64-fontcompiler! maybe this is a sign we should include this in the */
   /* base library... */
   switch (self->result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         TUX64_UNREACHABLE;
      case TUX64_ARGUMENTS_PARSE_STATUS_NOT_AN_ARGUMENT:
         TUX64_LOG_ERROR_FMT(
            "\'%.*s\' is not an argument",
            self->result.payload.not_an_argument.argument.characters,
            self->result.payload.not_an_argument.argument.ptr
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER:
         TUX64_LOG_ERROR_FMT(
            "unknown argument \'%.*s\'",
            self->result.payload.unknown_identifier.identifier.characters,
            self->result.payload.unknown_identifier.identifier.ptr
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING:
         TUX64_LOG_ERROR_FMT(
            "argument \'%.*s\' expects a parameter, but none was given",
            self->result.payload.parameter_missing.identifier.characters,
            self->result.payload.parameter_missing.identifier
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED:
         TUX64_LOG_ERROR_FMT(
            "argument \'%.*s\' doesn't expect parameter \'%.*s\'",
            self->result.payload.parameter_unexpected.identifier.characters,
            self->result.payload.parameter_unexpected.identifier.ptr,
            self->result.payload.parameter_unexpected.parameter.characters,
            self->result.payload.parameter_unexpected.parameter.ptr
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID:
         TUX64_LOG_ERROR_FMT(
            "invalid parameter \'%.*s\' for argument \'%.*s\': %.*s",
            self->result.payload.parameter_invalid.parameter.characters,
            self->result.payload.parameter_invalid.parameter.ptr,
            self->result.payload.parameter_invalid.identifier.characters,
            self->result.payload.parameter_invalid.identifier.ptr,
            self->result.payload.parameter_invalid.reason.characters,
            self->result.payload.parameter_invalid.reason.ptr
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_REQUIRED_MISSING:
         TUX64_LOG_ERROR_FMT(
            "missing required argument \'%.*s\'",
            self->result.payload.required_missing.identifier.characters,
            self->result.payload.required_missing.identifier.ptr
         );
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_textcompiler_exit_result_display_fs_error(
   const struct Tux64TextCompilerExitPayloadFsError * self
) {
   /* yes, i also copy+pasted this from mkrom (and fontcompiler!).  too bad! */
   switch (self->reason.status) {
      case TUX64_FS_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_FS_STATUS_NOT_FOUND:
         TUX64_LOG_ERROR("file not found");
         break;
      case TUX64_FS_STATUS_PERMISSION_DENIED:
         TUX64_LOG_ERROR("permission denied");
         break;
      case TUX64_FS_STATUS_NOT_A_FILE:
         TUX64_LOG_ERROR("expected a file, encountered a directory or other non-file object");
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         TUX64_UNREACHABLE;
         break;
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         TUX64_LOG_ERROR_FMT(
            "unknown I/O error (%d)",
            self->reason.payload.unknown_error
         );
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_textcompiler_exit_result_display_lexer_parse_error(
   struct Tux64TextCompilerExitPayloadLexerParseError * self
) {
   const char * msg;

   switch (self->reason.status) {
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK:
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OUT_OF_MEMORY:
         TUX64_UNREACHABLE;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_EMPTY_FILE:
         msg = "no text string entries provided";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_START_INVALID:
         msg = "invalid start to comment";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_END_UNMATCHED:
         msg = "non-existent comment ended";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_COMMENT_UNTERMINATED:
         msg = "comment unterminated";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_UNTERMINATED:
         msg = "unterminated string text entry";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_MALFORMED:
         msg = "invalid start to string text entry";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_IDENTIFIER:
         msg = "string entry field missing identifier";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_VALUE:
         msg = "string entry field missing value";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_SEPARATOR:
         msg = "string entry field missing separator";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_MISSING_NEWLINE:
         msg = "string entry field missing newline";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_INCORRECT_TYPE_IDENTIFIER:
         msg = "incorrect type for identifier field";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_ENTRY_FIELD_INCORRECT_TYPE_TEXT:
         msg = "incorrect type for text field";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_EMPTY:
         msg = "text field cannot be empty";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_TOO_LONG:
         msg = "text field is too long";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_UNTERMINATED:
         msg = "text field is unterminated";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_STRING_VALUE_BAD_CHARACTER:
         msg = "text field contains one or more disallowed characters";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_UNKNOWN_FIELD:
         msg = "unknown field";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MALFORMED_FIELD:
         msg = "malformed field";
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_MISSING_REQUIRED_FIELD:
         msg = "missing one or more required fields in text string entry";
         break;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_ERROR_FMT(
      "parse error (line %" PRIu32 ", column %" PRIu32 "): %s",
      self->reason.payload.err.location.line + TUX64_LITERAL_UINT32(1u),
      self->reason.payload.err.location.column + TUX64_LITERAL_UINT32(1u),
      msg
   );

   tux64_fs_file_unload(&self->file);
   return;
}

static void
tux64_textcompiler_exit_result_display(
   struct Tux64TextCompilerExitResult * self
) {
   switch (self->status) {
      case TUX64_TEXTCOMPILER_EXIT_STATUS_OK:
         break;
      case TUX64_TEXTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY:
         TUX64_LOG_ERROR("out of memory");
         break;
      case TUX64_TEXTCOMPILER_EXIT_STATUS_TOO_MANY_ARGUMENTS:
         tux64_textcompiler_exit_result_display_too_many_arguments(&self->payload.too_many_arguments);
         break;
      case TUX64_TEXTCOMPILER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR:
         tux64_textcompiler_exit_result_display_arguments_parse_error(&self->payload.arguments_parse_error);
         break;
      case TUX64_TEXTCOMPILER_EXIT_STATUS_FS_ERROR:
         tux64_textcompiler_exit_result_display_fs_error(&self->payload.fs_error);
         break;
      case TUX64_TEXTCOMPILER_EXIT_STATUS_LEXER_PARSE_ERROR:
         tux64_textcompiler_exit_result_display_lexer_parse_error(&self->payload.lexer_parse_error);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static char *
tux64_textcompiler_format_cstr(
   const struct Tux64String * string
) {
   char * cstr;

   cstr = (char *)malloc((string->characters + 1u)* sizeof(char));
   if (cstr == NULL) {
      return NULL;
   }

   tux64_memory_copy(
      cstr,
      string->ptr,
      string->characters * TUX64_LITERAL_UINT32(sizeof(char))
   );
   cstr[string->characters] = '\0';

   return cstr;
}

static struct Tux64TextCompilerExitResult
tux64_textcompiler_main_parsed_args(
   const struct Tux64TextCompilerArguments * args
) {
   struct Tux64TextCompilerExitResult result;
   char * path_input_cstr;
   struct Tux64FsFileLoadResult input_file_load_result;
   struct Tux64String input_file_string;
   struct Tux64TextCompilerLexerParseResult lexer_parse_result;
   struct Tux64TextCompilerFormatterGenerateResult formatter_generate_result;
   char * path_output_cstr;
   struct Tux64FsLoadedFile output_file;
   struct Tux64FsResult output_file_save_result;

   TUX64_LOG_INFO_FMT(
      "loading string text file from %.*s",
      args->path_input.characters,
      args->path_input.ptr
   );

   path_input_cstr = tux64_textcompiler_format_cstr(&args->path_input);
   if (path_input_cstr == NULL) {
      result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   input_file_load_result = tux64_fs_file_load(path_input_cstr);
   free(path_input_cstr);

   switch (input_file_load_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = input_file_load_result.status;
         result.payload.fs_error.reason.payload = input_file_load_result.payload.err;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   input_file_string.ptr = (const char *)input_file_load_result.payload.ok.data;
   input_file_string.characters = input_file_load_result.payload.ok.bytes;

   TUX64_LOG_INFO("parsing string text file");

   lexer_parse_result = tux64_textcompiler_lexer_parse(&input_file_string);
   switch (lexer_parse_result.status) {
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OK:
         break;
      case TUX64_TEXTCOMPILER_LEXER_PARSE_STATUS_OUT_OF_MEMORY:
         tux64_fs_file_unload(&input_file_load_result.payload.ok);
         result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      default:
         result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_LEXER_PARSE_ERROR;
         result.payload.lexer_parse_error.reason = lexer_parse_result;
         result.payload.lexer_parse_error.file = input_file_load_result.payload.ok;
         return result;
   }

   TUX64_LOG_INFO_FMT(
      "generating C source for %" PRIu32 " strings",
      lexer_parse_result.payload.ok.length
   );

   formatter_generate_result = tux64_textcompiler_formatter_generate(
      &lexer_parse_result.payload.ok
   );
   tux64_textcompiler_lexer_string_array_free(&lexer_parse_result.payload.ok);
   tux64_fs_file_unload(&input_file_load_result.payload.ok);

   switch (formatter_generate_result.status) {
      case TUX64_TEXTCOMPILER_FORMATTER_GENERATE_STATUS_OK:
         break;
      case TUX64_TEXTCOMPILER_FORMATTER_GENERATE_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_INFO_FMT(
      "writing generated C source code to %.*s",
      args->path_output.characters,
      args->path_output.ptr
   );

   path_output_cstr = tux64_textcompiler_format_cstr(&args->path_output);
   if (path_output_cstr == NULL) {
      result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
      free(formatter_generate_result.payload.ok.data);
      return result;
   }

   output_file.data = formatter_generate_result.payload.ok.data;
   output_file.bytes = formatter_generate_result.payload.ok.bytes;

   output_file_save_result = tux64_fs_file_save(
      path_output_cstr,
      &output_file
   );
   free(path_output_cstr);
   free(formatter_generate_result.payload.ok.data);

   switch (output_file_save_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = input_file_load_result.status;
         result.payload.fs_error.reason.payload = input_file_load_result.payload.err;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_OK;
   return result;
}

static struct Tux64TextCompilerExitResult
tux64_textcompiler_main(
   Tux64UInt8 argc,
   const char * const * argv
) {
   struct Tux64TextCompilerExitResult result;
   struct Tux64ArgumentsIterator args_iterator;
   struct Tux64ArgumentsParseResult args_parse_result;
   struct Tux64TextCompilerArguments args_parsed;

   /* display the help menu if no arguments are given */
   if (argc == TUX64_LITERAL_UINT8(1u)) {
      tux64_textcompiler_arguments_print_menu_help();
      result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_OK;
      return result;
   }

   /* skip over argv[0] for the arguments iterator */
   tux64_arguments_iterator_initialize_command_line(
      &args_iterator,
      argc - TUX64_LITERAL_UINT8(1u),
      &argv[1u]
   );

   args_parse_result = tux64_textcompiler_arguments_parse(
      &args_iterator,
      &args_parsed
   );
   switch (args_parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_OK;
         return result;
      default:
         result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         result.payload.arguments_parse_error.result = args_parse_result;
         return result;
   }

   return tux64_textcompiler_main_parsed_args(&args_parsed);
}

int main(int argc, char ** argv) {
   struct Tux64TextCompilerExitResult exit_result;

   if (argc > TUX64_LITERAL_UINT8(TUX64_TEXTCOMPILER_ARGC_MAX)) {
      exit_result.status = TUX64_TEXTCOMPILER_EXIT_STATUS_TOO_MANY_ARGUMENTS;
      exit_result.payload.too_many_arguments.argc_given = argc;
      exit_result.payload.too_many_arguments.argc_max = TUX64_LITERAL_UINT8(TUX64_TEXTCOMPILER_ARGC_MAX);
      goto exit;
   }

   exit_result = tux64_textcompiler_main(
      (Tux64UInt8)argc,
      (const char * const *)argv
   );

exit:
   tux64_textcompiler_exit_result_display(&exit_result);
   return (int)exit_result.status;
}

