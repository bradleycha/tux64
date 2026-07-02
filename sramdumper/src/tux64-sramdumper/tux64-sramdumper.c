/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-sramdumper/tux64-sramdumper.c - Main application           */
/*    entrypoint for tux64-sramdumper.                                        */
/*----------------------------------------------------------------------------*/

#include "tux64-sramdumper/tux64-sramdumper.h"

#include <tux64/log.h>
#include <tux64/arguments.h>
#include <tux64/memory.h>
#include <tux64/fs.h>
#include "tux64-sramdumper/arguments.h"
#include "tux64-sramdumper/parse.h"
#include "tux64-sramdumper/format.h"

#include <stdlib.h>
#include <inttypes.h>

#define TUX64_SRAMDUMPER_ARGC_MAX \
   TUX64_UINT8_MAX

enum Tux64SramDumperExitStatus {
   TUX64_SRAMDUMPER_EXIT_STATUS_OK = 0u,
   TUX64_SRAMDUMPER_EXIT_STATUS_OUT_OF_MEMORY,
   TUX64_SRAMDUMPER_EXIT_STATUS_TOO_MANY_ARGUMENTS,
   TUX64_SRAMDUMPER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR,
   TUX64_SRAMDUMPER_EXIT_STATUS_FS_ERROR,
   TUX64_SRAMDUMPER_EXIT_STATUS_PARSE_ERROR
};

struct Tux64SramDumperExitPayloadTooManyArguments {
   int argc_given;
   Tux64UInt8 argc_max;
};

struct Tux64SramDumperExitPayloadArgumentsParseError {
   struct Tux64ArgumentsParseResult result;
};

struct Tux64SramDumperExitPayloadFsError {
   struct Tux64FsResult reason;
};

struct Tux64SramDumperExitPayloadParseError {
   struct Tux64SramDumperParseResult reason;
};

union Tux64SramDumperExitPayload {
   struct Tux64SramDumperExitPayloadTooManyArguments too_many_arguments;
   struct Tux64SramDumperExitPayloadArgumentsParseError arguments_parse_error;
   struct Tux64SramDumperExitPayloadFsError fs_error;
   struct Tux64SramDumperExitPayloadParseError parse_error;
};

struct Tux64SramDumperExitResult {
   enum Tux64SramDumperExitStatus status;
   union Tux64SramDumperExitPayload payload;
};

static void
tux64_sramdumper_exit_result_display_too_many_arguments(
   const struct Tux64SramDumperExitPayloadTooManyArguments * self
) {
   TUX64_LOG_ERROR_FMT(
      "unable to handle %d arguments at once, maximum is %" PRIu8,
      self->argc_given,
      self->argc_max
   );
   return;
}

static void
tux64_sramdumepr_exit_result_display_parse_error(
   const struct Tux64SramDumperExitPayloadParseError * self
) {
   switch (self->reason.status) {
      case TUX64_SRAMDUMPER_PARSE_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_SRAMDUMPER_PARSE_STATUS_INVALID_DUMP:
         TUX64_LOG_ERROR("invalid SRAM dump");
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_sramdumper_exit_result_display(
   const struct Tux64SramDumperExitResult * self
) {
   switch (self->status) {
      case TUX64_SRAMDUMPER_EXIT_STATUS_OK:
         break;
      case TUX64_SRAMDUMPER_EXIT_STATUS_OUT_OF_MEMORY:
         TUX64_LOG_ERROR("out of memory");
         break;
      case TUX64_SRAMDUMPER_EXIT_STATUS_TOO_MANY_ARGUMENTS:
         tux64_sramdumper_exit_result_display_too_many_arguments(&self->payload.too_many_arguments);
         break;
      case TUX64_SRAMDUMPER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR:
         tux64_arguments_log_result(&self->payload.arguments_parse_error.result);
         break;
      case TUX64_SRAMDUMPER_EXIT_STATUS_FS_ERROR:
         tux64_fs_log_result(&self->payload.fs_error.reason);
         break;
      case TUX64_SRAMDUMPER_EXIT_STATUS_PARSE_ERROR:
         tux64_sramdumepr_exit_result_display_parse_error(&self->payload.parse_error);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static char *
tux64_sramdumper_format_cstr(
   const struct Tux64String *string
) {
   char * cstr;

   cstr = (char *)malloc((string->characters + 1u) * sizeof(char));
   if (cstr == NULL) return NULL;

   tux64_memory_copy(
      cstr,
      string->ptr,
      string->characters * TUX64_LITERAL_UINT32(sizeof(char))
   );
   cstr[string->characters] = '\0';

   return cstr;
}

static struct Tux64SramDumperExitResult
tux64_sramdumper_main_parsed_args(
   const struct Tux64SramDumperArguments * args
) {
   struct Tux64SramDumperExitResult result;
   char * path_input_cstr;
   struct Tux64FsFileLoadResult input_file_load_result;
   struct Tux64SramDumperParseResult parse_result;
   struct Tux64SramDumperFormatResult format_result;
   char * path_output_cstr;
   struct Tux64FsLoadedFile output_file;
   struct Tux64FsResult output_file_save_result;

   TUX64_LOG_INFO_FMT(
      "loading SRAM dump from %.*s",
      args->path_input.characters,
      args->path_input.ptr
   );

   path_input_cstr = tux64_sramdumper_format_cstr(&args->path_input);
   if (path_input_cstr == NULL) {
      result.status = TUX64_SRAMDUMPER_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   input_file_load_result = tux64_fs_file_load(path_input_cstr);
   free(path_input_cstr);

   switch (input_file_load_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_SRAMDUMPER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_SRAMDUMPER_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = input_file_load_result.status;
         result.payload.fs_error.reason.payload = input_file_load_result.payload.err;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_INFO("parsing SRAM dump");

   parse_result = tux64_sramdumper_parse(
      input_file_load_result.payload.ok.data,
      input_file_load_result.payload.ok.bytes
   );
   tux64_fs_file_unload(&input_file_load_result.payload.ok);

   switch (parse_result.status) {
      case TUX64_SRAMDUMPER_PARSE_STATUS_OK:
         break;
      case TUX64_SRAMDUMPER_PARSE_STATUS_INVALID_DUMP:
         result.status = TUX64_SRAMDUMPER_EXIT_STATUS_PARSE_ERROR;
         result.payload.parse_error.reason = parse_result;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_INFO("formatting SRAM dump as text");

   format_result = tux64_sramdumper_format(&parse_result.payload.ok);

   TUX64_LOG_INFO_FMT(
      "writing parsed SRAM dump to %.*s",
      args->path_output.characters,
      args->path_output.ptr
   );

   path_output_cstr = tux64_sramdumper_format_cstr(&args->path_output);
   if (path_output_cstr == NULL) {
      result.status = TUX64_SRAMDUMPER_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   output_file.data = (Tux64UInt8 *)format_result.data;
   output_file.bytes = format_result.characters * TUX64_LITERAL_UINT32(sizeof(char));

   output_file_save_result = tux64_fs_file_save(
      path_output_cstr,
      &output_file
   );
   free(path_output_cstr);

   switch (output_file_save_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_SRAMDUMPER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_SRAMDUMPER_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = input_file_load_result.status;
         result.payload.fs_error.reason.payload = input_file_load_result.payload.err;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   result.status = TUX64_SRAMDUMPER_EXIT_STATUS_OK;
   return result;
}

static struct Tux64SramDumperExitResult
tux64_sramdumper_main(
   Tux64UInt8 argc,
   const char * const * argv
) {
   struct Tux64SramDumperExitResult result;
   struct Tux64ArgumentsIterator args_iterator;
   struct Tux64ArgumentsParseResult args_parse_result;
   struct Tux64SramDumperArguments args_parsed;

   if (argc == TUX64_LITERAL_UINT8(1u)) {
      tux64_sramdumper_arguments_print_menu_help();
      result.status = TUX64_SRAMDUMPER_EXIT_STATUS_OK;
      return result;
   }

   tux64_arguments_iterator_initialize_command_line(
      &args_iterator,
      argc - TUX64_LITERAL_UINT8(1u),
      &argv[1u]
   );

   args_parse_result = tux64_sramdumper_arguments_parse(
      &args_iterator,
      &args_parsed
   );
   switch (args_parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         result.status = TUX64_SRAMDUMPER_EXIT_STATUS_OK;
         return result;
      default:
         result.status = TUX64_SRAMDUMPER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         result.payload.arguments_parse_error.result = args_parse_result;
         return result;
   }

   return tux64_sramdumper_main_parsed_args(&args_parsed);
}

int main(int argc, char ** argv) {
   struct Tux64SramDumperExitResult exit_result;

   if (argc > TUX64_LITERAL_UINT8(TUX64_SRAMDUMPER_ARGC_MAX)) {
      exit_result.status = TUX64_SRAMDUMPER_EXIT_STATUS_TOO_MANY_ARGUMENTS;
      exit_result.payload.too_many_arguments.argc_given = argc;
      exit_result.payload.too_many_arguments.argc_max = TUX64_LITERAL_UINT8(TUX64_SRAMDUMPER_ARGC_MAX);
      goto exit;
   }

   exit_result = tux64_sramdumper_main(
      (Tux64UInt8)argc,
      (const char * const *)argv
   );

exit:
   tux64_sramdumper_exit_result_display(&exit_result);
   return (int)exit_result.status;
}

