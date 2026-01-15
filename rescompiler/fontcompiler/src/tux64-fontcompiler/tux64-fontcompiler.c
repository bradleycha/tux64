/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/fontcompiler/src/tux64-fontcompiler/tux64-fontcompiler.c -     */
/*    Main application entrypoint for tux64-fontcompiler.                     */
/*----------------------------------------------------------------------------*/

#include "tux64-fontcompiler/tux64-fontcompiler.h"

#include <tux64/log.h>
#include <tux64/arguments.h>
#include <tux64/memory.h>
#include <tux64/fs.h>
#include "tux64-fontcompiler/arguments.h"
#include "tux64-fontcompiler/generator.h"

#include <stdlib.h>
#include <inttypes.h>

#define TUX64_FONTCOMPILER_ARGC_MAX TUX64_UINT8_MAX

enum Tux64FontCompilerExitStatus {
   TUX64_FONTCOMPILER_EXIT_STATUS_OK = 0u,
   TUX64_FONTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY,
   TUX64_FONTCOMPILER_EXIT_STATUS_TOO_MANY_ARGUMENTS,
   TUX64_FONTCOMPILER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR,
   TUX64_FONTCOMPILER_EXIT_STATUS_FS_ERROR,
   TUX64_FONTCOMPILER_EXIT_STATUS_GENERATOR_PARSE_ERROR
};

struct Tux64FontCompilerExitPayloadTooManyArguments {
   int argc_given;
   Tux64UInt8 argc_max;
};

struct Tux64FontCompilerExitPayloadArgumentsParseError {
   struct Tux64ArgumentsParseResult result;
};

struct Tux64FontCompilerExitPayloadFsError {
   struct Tux64FsResult reason;
};

struct Tux64FontCompilerExitPayloadGeneratorParseError {
   struct Tux64FontCompilerGeneratorParseResult reason;
};

union Tux64FontCompilerExitPayload {
   struct Tux64FontCompilerExitPayloadTooManyArguments too_many_arguments;
   struct Tux64FontCompilerExitPayloadArgumentsParseError arguments_parse_error;
   struct Tux64FontCompilerExitPayloadFsError fs_error;
   struct Tux64FontCompilerExitPayloadGeneratorParseError generator_parse_error;
};

struct Tux64FontCompilerExitResult {
   enum Tux64FontCompilerExitStatus status;
   union Tux64FontCompilerExitPayload payload;
};

static void
tux64_fontcompiler_exit_result_display_too_many_arguments(
   const struct Tux64FontCompilerExitPayloadTooManyArguments * self
) {
   TUX64_LOG_ERROR_FMT(
      "unable to handle %d arguments at once, maximum is %" PRIu8,
      self->argc_given,
      self->argc_max
   );
   return;
}

static void
tux64_fontcompiler_exit_result_display_arguments_parse_error(
   const struct Tux64FontCompilerExitPayloadArgumentsParseError * self
) {
   /* yes, I copy-pasted this from tux64-mkrom. deal with it. */
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
tux64_fontcompiler_exit_result_display_fs_error(
   const struct Tux64FontCompilerExitPayloadFsError * self
) {
   /* yes, i also copy+pasted this from mkrom.  too bad! */
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
tux64_fontcompiler_exit_result_display_generator_parse_error(
   const struct Tux64FontCompilerExitPayloadGeneratorParseError * self
) {
   switch (self->reason.status) {
      case TUX64_FONTCOMPILER_GENERATOR_PARSE_STATUS_OK:
      case TUX64_FONTCOMPILER_GENERATOR_PARSE_STATUS_OUT_OF_MEMORY:
         TUX64_UNREACHABLE;
      case TUX64_FONTCOMPILER_GENERATOR_PARSE_STATUS_INVALID_IMAGE_FORMAT:
         TUX64_LOG_ERROR("invalid image data.  please refer to the help menu for information on the expected input image format");
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_fontcompiler_exit_result_display(
   const struct Tux64FontCompilerExitResult * self
) {
   switch (self->status) {
      case TUX64_FONTCOMPILER_EXIT_STATUS_OK:
         break;
      case TUX64_FONTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY:
         TUX64_LOG_ERROR("out of memory");
         break;
      case TUX64_FONTCOMPILER_EXIT_STATUS_TOO_MANY_ARGUMENTS:
         tux64_fontcompiler_exit_result_display_too_many_arguments(&self->payload.too_many_arguments);
         break;
      case TUX64_FONTCOMPILER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR:
         tux64_fontcompiler_exit_result_display_arguments_parse_error(&self->payload.arguments_parse_error);
         break;
      case TUX64_FONTCOMPILER_EXIT_STATUS_FS_ERROR:
         tux64_fontcompiler_exit_result_display_fs_error(&self->payload.fs_error);
         break;
      case TUX64_FONTCOMPILER_EXIT_STATUS_GENERATOR_PARSE_ERROR:
         tux64_fontcompiler_exit_result_display_generator_parse_error(&self->payload.generator_parse_error);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static char *
tux64_fontcompiler_format_cstr(
   const struct Tux64String * string
) {
   char * cstr;

   cstr = (char *)malloc((string->characters + 1u) * sizeof(char));
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

static struct Tux64FontCompilerExitResult
tux64_fontcompiler_main_parsed_args(
   const struct Tux64FontCompilerArguments * args
) {
   struct Tux64FontCompilerExitResult result;
   char * path_input_cstr;
   struct Tux64FsFileLoadResult input_file_load_result;
   struct Tux64FontCompilerGeneratorParseResult generator_parse_result;
   char * path_output_cstr;
   struct Tux64FsLoadedFile output_file;
   struct Tux64FsResult output_file_save_result;

   TUX64_LOG_INFO_FMT(
      "loading fontmap from %.*s",
      args->path_input.characters,
      args->path_input.ptr
   );

   /* have to do this crap because libc stinks... */
   path_input_cstr = tux64_fontcompiler_format_cstr(&args->path_input);
   if (path_input_cstr == NULL) {
      result.status = TUX64_FONTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   input_file_load_result = tux64_fs_file_load(path_input_cstr);
   free(path_input_cstr);

   switch (input_file_load_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_FONTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_FONTCOMPILER_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = input_file_load_result.status;
         result.payload.fs_error.reason.payload = input_file_load_result.payload.err;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_INFO_FMT(
      "compiling %" PRIu32 " bytes of image data to C source code",
      input_file_load_result.payload.ok.bytes
   );

   generator_parse_result = tux64_fontcompiler_generator_parse(
      input_file_load_result.payload.ok.data,
      input_file_load_result.payload.ok.bytes,
      &args->name
   );
   tux64_fs_file_unload(&input_file_load_result.payload.ok);

   switch (generator_parse_result.status) {
      case TUX64_FONTCOMPILER_GENERATOR_PARSE_STATUS_OK:
         break;
      case TUX64_FONTCOMPILER_GENERATOR_PARSE_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_FONTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      case TUX64_FONTCOMPILER_GENERATOR_PARSE_STATUS_INVALID_IMAGE_FORMAT:
         result.status = TUX64_FONTCOMPILER_EXIT_STATUS_GENERATOR_PARSE_ERROR;
         result.payload.generator_parse_error.reason = generator_parse_result;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_INFO_FMT(
      "done, writing C source code to %.*s",
      args->path_output.characters,
      args->path_output.ptr
   );

   /* have to do this crap again */
   path_output_cstr = tux64_fontcompiler_format_cstr(&args->path_output);
   if (path_output_cstr == NULL) {
      result.status = TUX64_FONTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
      goto exit;
   }

   output_file.data = generator_parse_result.payload.ok.data;
   output_file.bytes = generator_parse_result.payload.ok.bytes;
   output_file_save_result = tux64_fs_file_save(
      path_output_cstr,
      &output_file
   );
   free(path_output_cstr);

   switch (output_file_save_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_FONTCOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_FONTCOMPILER_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = output_file_save_result.status;
         result.payload.fs_error.reason.payload = output_file_save_result.payload;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   result.status = TUX64_FONTCOMPILER_EXIT_STATUS_OK;
exit:
   free(generator_parse_result.payload.ok.data);
   return result;
}

static struct Tux64FontCompilerExitResult
tux64_fontcompiler_main(
   Tux64UInt8 argc,
   const char * const * argv
) {
   struct Tux64FontCompilerExitResult result;
   struct Tux64ArgumentsIterator args_iterator;
   struct Tux64ArgumentsParseResult args_parse_result;
   struct Tux64FontCompilerArguments args_parsed;

   /* display the help menu if no arguments are given */
   if (argc == TUX64_LITERAL_UINT8(1u)) {
      tux64_fontcompiler_arguments_print_menu_help();
      result.status = TUX64_FONTCOMPILER_EXIT_STATUS_OK;
      return result;
   }
   
   /* skip over argv[0] for the arguments iterator */
   tux64_arguments_iterator_initialize_command_line(
      &args_iterator,
      argc - TUX64_LITERAL_UINT8(1u),
      &argv[1u]
   );

   args_parse_result = tux64_fontcompiler_arguments_parse(
      &args_iterator,
      &args_parsed
   );
   switch (args_parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         result.status = TUX64_FONTCOMPILER_EXIT_STATUS_OK;
         return result;
      default:
         result.status = TUX64_FONTCOMPILER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         result.payload.arguments_parse_error.result = args_parse_result;
         return result;
   }

   return tux64_fontcompiler_main_parsed_args(&args_parsed);
}

int main(int argc, char ** argv) {
   struct Tux64FontCompilerExitResult exit_result;

   if (argc > TUX64_LITERAL_UINT8(TUX64_FONTCOMPILER_ARGC_MAX)) {
      exit_result.status = TUX64_FONTCOMPILER_EXIT_STATUS_TOO_MANY_ARGUMENTS;
      exit_result.payload.too_many_arguments.argc_given = argc;
      exit_result.payload.too_many_arguments.argc_max = TUX64_LITERAL_UINT8(TUX64_FONTCOMPILER_ARGC_MAX);
      goto exit;
   }

   exit_result = tux64_fontcompiler_main(
      (Tux64UInt8)argc,
      (const char * const *)argv
   );

exit:
   tux64_fontcompiler_exit_result_display(&exit_result);
   return (int)exit_result.status;
}

