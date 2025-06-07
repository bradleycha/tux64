/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/tux64-mkrom.c - Main application entrypoint for      */
/*    tux64-mkrom.                                                            */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"

#include <tux64/log.h>
#include <tux64/arguments.h>
#include "tux64-mkrom/arguments.h"

#include <inttypes.h>

#define TUX64_MKROM_ARGC_MAX TUX64_UINT8_MAX

enum Tux64MkromExitStatus {
   TUX64_MKROM_EXIT_STATUS_OK = 0u,
   TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS,
   TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR
};

struct Tux64MkromExitPayloadTooManyArguments {
   int argc_given;
   Tux64UInt8 argc_max;
};

struct Tux64MkromExitPayloadArgumentsParseError {
   struct Tux64ArgumentsParseResult result;
};

union Tux64MkromExitPayload {
   struct Tux64MkromExitPayloadTooManyArguments too_many_arguments;
   struct Tux64MkromExitPayloadArgumentsParseError arguments_parse_error;
};

struct Tux64MkromExitResult {
   enum Tux64MkromExitStatus status;
   union Tux64MkromExitPayload payload;
};

static void
tux64_mkrom_exit_result_display_too_many_arguments(
   const struct Tux64MkromExitPayloadTooManyArguments * self
) {
   TUX64_LOG_ERROR_FMT(
      "unable to handle %d arguments at once, maximum is %" PRIu8,
      self->argc_given,
      self->argc_max
   );
   return;
}

static void
tux64_mkrom_exit_result_display_arguments_parse_error(
   const struct Tux64MkromExitPayloadArgumentsParseError * self
) {
   switch (self->result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         TUX64_UNREACHABLE;
      case TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER:
         TUX64_LOG_ERROR_FMT(
            "unknown argument \'%s\'",
            self->result.payload.unknown_identifier.identifier
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING:
         TUX64_LOG_ERROR_FMT(
            "argument \'%s\' expects a parameter, but none was given",
            self->result.payload.parameter_missing.identifier
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED:
         TUX64_LOG_ERROR_FMT(
            "argument \'%s\' doesn't expect parameter \'%s\'",
            self->result.payload.parameter_unexpected.identifier,
            self->result.payload.parameter_unexpected.parameter
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID:
         TUX64_LOG_ERROR_FMT(
            "invalid parameter \'%s\' for argument \'%s\': %s",
            self->result.payload.parameter_invalid.parameter,
            self->result.payload.parameter_invalid.identifier,
            self->result.payload.parameter_invalid.reason
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_REQUIRED_MISSING:
         TUX64_LOG_ERROR_FMT(
            "missing required argument \'%s\'",
            self->result.payload.required_missing.identifier
         );
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_mkrom_exit_result_display(
   const struct Tux64MkromExitResult * self
) {
   switch (self->status) {
      case TUX64_MKROM_EXIT_STATUS_OK:
         break;
      case TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS:
         tux64_mkrom_exit_result_display_too_many_arguments(&self->payload.too_many_arguments);
         break;
      case TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR:
         tux64_mkrom_exit_result_display_arguments_parse_error(&self->payload.arguments_parse_error);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static struct Tux64MkromExitResult
tux64_mkrom_main(
   Tux64UInt8 argc,
   const char * const * argv
) {
   struct Tux64MkromExitResult result;
   struct Tux64ArgumentsIterator args_iterator;
   struct Tux64ArgumentsParseResult args_parse_result;
   struct Tux64MkromArguments args;

   /* if the user is completely clueless, show them the help menu when no */
   /* arguments are present */
   if (argc == TUX64_LITERAL_UINT8(1u)) {
      tux64_mkrom_arguments_print_menu_help();
      result.status = TUX64_MKROM_EXIT_STATUS_OK;
      return result;
   }

   /* makes sure to skip over argv[0] */
   tux64_arguments_iterator_initialize_command_line(
      &args_iterator,
      argc - TUX64_LITERAL_UINT8(1u),
      &argv[1]
   );

   args_parse_result = tux64_mkrom_arguments_parse(
      &args_iterator,
      &args
   );
   switch (args_parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         result.status = TUX64_MKROM_EXIT_STATUS_OK;
         return result;
      default:
         result.status = TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         result.payload.arguments_parse_error.result = args_parse_result;
         return result;
   }

   /* TODO: implement */
   (void)args;
   TUX64_LOG_INFO("hello from tux64_mkrom_main()!");
   result.status = TUX64_MKROM_EXIT_STATUS_OK;
   return result;
}

int main(int argc, char ** argv) {
   struct Tux64MkromExitResult exit_result;

   if (argc > TUX64_LITERAL_UINT8(TUX64_MKROM_ARGC_MAX)) {
      exit_result.status = TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS;
      exit_result.payload.too_many_arguments.argc_given = argc;
      exit_result.payload.too_many_arguments.argc_max = TUX64_LITERAL_UINT8(TUX64_MKROM_ARGC_MAX);
      goto exit;
   }

   exit_result = tux64_mkrom_main(
      (Tux64UInt8)argc,
      (const char * const *)argv
   );

exit:
   tux64_mkrom_exit_result_display(&exit_result);
   return (int)exit_result.status;
}

