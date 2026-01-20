/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/imagecompiler/src/tux64-imagecompiler/tux64-imagecompiler.c -  */
/*    Main application entrypoint for tux64-imagecompiler.                    */
/*----------------------------------------------------------------------------*/

#include "tux64-imagecompiler/tux64-imagecompiler.h"

#include <tux64/log.h>
#include <tux64/arguments.h>
#include <tux64/memory.h>
#include <tux64/fs.h>
#include "tux64-imagecompiler/arguments.h"
#include "tux64-imagecompiler/encoder.h"
#include "tux64-imagecompiler/compressor.h"
#include "tux64-imagecompiler/formatter.h"

#include <stdlib.h>
#include <inttypes.h>

#define TUX64_IMAGECOMPILER_ARGC_MAX TUX64_UINT8_MAX

enum Tux64ImageCompilerExitStatus {
   TUX64_IMAGECOMPILER_EXIT_STATUS_OK = 0u,
   TUX64_IMAGECOMPILER_EXIT_STATUS_OUT_OF_MEMORY,
   TUX64_IMAGECOMPILER_EXIT_STATUS_TOO_MANY_ARGUMENTS,
   TUX64_IMAGECOMPILER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR,
   TUX64_IMAGECOMPILER_EXIT_STATUS_FS_ERROR,
   TUX64_IMAGECOMPILER_EXIT_STATUS_ENCODE_ERROR
};

struct Tux64ImageCompilerExitPayloadTooManyArguments {
   int argc_given;
   Tux64UInt8 argc_max;
};

struct Tux64ImageCompilerExitPayloadArgumentsParseError {
   struct Tux64ArgumentsParseResult result;
};

struct Tux64ImageCompilerExitPayloadFsError {
   struct Tux64FsResult reason;
};

struct Tux64ImageCompilerExitPayloadEncodeError {
   enum Tux64ImageCompilerEncoderStatus reason;
};

union Tux64ImageCompilerExitPayload {
   struct Tux64ImageCompilerExitPayloadTooManyArguments too_many_arguments;
   struct Tux64ImageCompilerExitPayloadArgumentsParseError arguments_parse_error;
   struct Tux64ImageCompilerExitPayloadFsError fs_error;
   struct Tux64ImageCompilerExitPayloadEncodeError encode_error;
};

struct Tux64ImageCompilerExitResult {
   enum Tux64ImageCompilerExitStatus status;
   union Tux64ImageCompilerExitPayload payload;
};

static void
tux64_imagecompiler_exit_result_display_too_many_arguments(
   const struct Tux64ImageCompilerExitPayloadTooManyArguments * self
) {
   TUX64_LOG_ERROR_FMT(
      "unable to handle %d arguments at once, maximum is %" PRIu8,
      self->argc_given,
      self->argc_max
   );
   return;
}

static void
tux64_imagecompiler_exit_result_display_arguments_parse_error(
   const struct Tux64ImageCompilerExitPayloadArgumentsParseError * self
) {
   /* i'm starting to think we should make this a dedicated library function */
   /* instead of copy+pasting this everywhere... */
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
tux64_imagecompiler_exit_result_display_fs_error(
   const struct Tux64ImageCompilerExitPayloadFsError * self
) {
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
tux64_imagecompiler_exit_result_display_encode_error(
   const struct Tux64ImageCompilerExitPayloadEncodeError * self
) {
   switch (self->reason) {
      case TUX64_IMAGECOMPILER_ENCODER_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_IMAGECOMPILER_ENCODER_STATUS_INVALID_IMAGE_FORMAT:
         TUX64_LOG_ERROR("image is not formatted as 64x64 RGBA8888 pixel data");
         break;
      case TUX64_IMAGECOMPILER_ENCODER_STATUS_TOO_MANY_COLORS:
         TUX64_LOG_ERROR("image has too many unique colors");
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_imagecompiler_exit_result_display(
   const struct Tux64ImageCompilerExitResult * self
) {
   switch (self->status) {
      case TUX64_IMAGECOMPILER_EXIT_STATUS_OK:
         break;
      case TUX64_IMAGECOMPILER_EXIT_STATUS_OUT_OF_MEMORY:
         TUX64_LOG_ERROR("out of memory");
         break;
      case TUX64_IMAGECOMPILER_EXIT_STATUS_TOO_MANY_ARGUMENTS:
         tux64_imagecompiler_exit_result_display_too_many_arguments(&self->payload.too_many_arguments);
         break;
      case TUX64_IMAGECOMPILER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR:
         tux64_imagecompiler_exit_result_display_arguments_parse_error(&self->payload.arguments_parse_error);
         break;
      case TUX64_IMAGECOMPILER_EXIT_STATUS_FS_ERROR:
         tux64_imagecompiler_exit_result_display_fs_error(&self->payload.fs_error);
         break;
      case TUX64_IMAGECOMPILER_EXIT_STATUS_ENCODE_ERROR:
         tux64_imagecompiler_exit_result_display_encode_error(&self->payload.encode_error);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static char *
tux64_imagecompiler_format_cstr(
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

/* these are global because they can use a lot of memory, and we don't want */
/* to risk stack overflow. */
struct Tux64ImageCompilerEncoderImageData tux64_imagecompiler_image_encoded;
struct Tux64ImageCompilerCompressorImageData tux64_imagecompiler_image_compressed;

static struct Tux64ImageCompilerExitResult
tux64_imagecompiler_main_parsed_args(
   const struct Tux64ImageCompilerArguments * args
) {
   struct Tux64ImageCompilerExitResult result;
   char * path_input_cstr;
   struct Tux64FsFileLoadResult input_file_load_result;
   enum Tux64ImageCompilerEncoderStatus image_encode_status;
   double compression_ratio;
   struct Tux64ImageCompilerFormatterGenerateResult image_format_result;
   char * path_output_cstr;
   struct Tux64FsLoadedFile output_file;
   struct Tux64FsResult output_file_save_result;

   TUX64_LOG_INFO_FMT(
      "loading image data from %.*s",
      args->path_input.characters,
      args->path_input.ptr
   );

   path_input_cstr = tux64_imagecompiler_format_cstr(&args->path_input);
   if (path_input_cstr == NULL) {
      result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   input_file_load_result = tux64_fs_file_load(path_input_cstr);
   free(path_input_cstr);

   switch (input_file_load_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = input_file_load_result.status;
         result.payload.fs_error.reason.payload = input_file_load_result.payload.err;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_INFO("encoding image data to CI4/RGBA5551");

   image_encode_status = tux64_imagecompiler_encoder_encode(
      input_file_load_result.payload.ok.data,
      input_file_load_result.payload.ok.bytes,
      &tux64_imagecompiler_image_encoded
   );

   tux64_fs_file_unload(&input_file_load_result.payload.ok);

   switch (image_encode_status) {
      case TUX64_IMAGECOMPILER_ENCODER_STATUS_OK:
         break;
      case TUX64_IMAGECOMPILER_ENCODER_STATUS_INVALID_IMAGE_FORMAT:
      case TUX64_IMAGECOMPILER_ENCODER_STATUS_TOO_MANY_COLORS:
         result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_ENCODE_ERROR;
         result.payload.encode_error.reason = image_encode_status;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_INFO("compressing image data");

   tux64_imagecompiler_image_compressed = tux64_imagecompiler_compressor_compress(
      &tux64_imagecompiler_image_encoded
   );

   compression_ratio = ((Tux64Float64)tux64_imagecompiler_image_compressed.length / (Tux64Float64)TUX64_LITERAL_UINT32(sizeof(tux64_imagecompiler_image_encoded.pixel_tuples)));

   TUX64_LOG_INFO_FMT(
      "image data compressed to %" PRIu32 " bytes (%.1f%% of original size)",
      tux64_imagecompiler_image_compressed.length,
      compression_ratio * TUX64_LITERAL_FLOAT64(100.0)
   );

   TUX64_LOG_INFO("generating C source code");

   image_format_result = tux64_imagecompiler_formatter_generate(
      &tux64_imagecompiler_image_compressed,
      tux64_imagecompiler_image_encoded.color_table,
      &args->name_pixels,
      &args->name_color_table
   );

   switch (image_format_result.status) {
      case TUX64_IMAGECOMPILER_FORMATTER_GENERATE_STATUS_OK:
         break;
      case TUX64_IMAGECOMPILER_FORMATTER_GENERATE_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_INFO_FMT(
      "writing generated C source code to %.*s",
      args->path_output.characters,
      args->path_output.ptr
   );

   path_output_cstr = tux64_imagecompiler_format_cstr(&args->path_output);
   if (path_output_cstr == NULL) {
      result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   output_file.data = image_format_result.payload.ok.data;
   output_file.bytes = image_format_result.payload.ok.bytes;

   output_file_save_result = tux64_fs_file_save(
      path_output_cstr,
      &output_file
   );
   free(path_output_cstr);
   free(image_format_result.payload.ok.data);
   
   switch (output_file_save_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = input_file_load_result.status;
         result.payload.fs_error.reason.payload = input_file_load_result.payload.err;
         return result;
      default:
         TUX64_UNREACHABLE;
   }

   result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_OK;
   return result;
}

static struct Tux64ImageCompilerExitResult
tux64_imagecompiler_main(
   Tux64UInt8 argc,
   const char * const * argv
) {
   struct Tux64ImageCompilerExitResult result;
   struct Tux64ArgumentsIterator args_iterator;
   struct Tux64ArgumentsParseResult args_parse_result;
   struct Tux64ImageCompilerArguments args_parsed;

   if (argc == TUX64_LITERAL_UINT8(1u)) {
      tux64_imagecompiler_arguments_print_menu_help();
      result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_OK;
      return result;
   }

   tux64_arguments_iterator_initialize_command_line(
      &args_iterator,
      argc - TUX64_LITERAL_UINT8(1u),
      &argv[1u]
   );

   args_parse_result = tux64_imagecompiler_arguments_parse(
      &args_iterator,
      &args_parsed
   );
   switch (args_parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_OK;
         return result;
      default:
         result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         result.payload.arguments_parse_error.result = args_parse_result;
         return result;
   }
   
   return tux64_imagecompiler_main_parsed_args(&args_parsed);
}

int main(int argc, char ** argv) {
   struct Tux64ImageCompilerExitResult exit_result;

   if (argc > TUX64_LITERAL_UINT8(TUX64_IMAGECOMPILER_ARGC_MAX)) {
      exit_result.status = TUX64_IMAGECOMPILER_EXIT_STATUS_TOO_MANY_ARGUMENTS;
      exit_result.payload.too_many_arguments.argc_given = argc;
      exit_result.payload.too_many_arguments.argc_max = TUX64_LITERAL_UINT8(TUX64_IMAGECOMPILER_ARGC_MAX);
      goto exit;
   }

   exit_result = tux64_imagecompiler_main(
      (Tux64UInt8)argc,
      (const char * const *)argv
   );

exit:
   tux64_imagecompiler_exit_result_display(&exit_result);
   return (int)exit_result.status;
}

