/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/tux64-mkrom.c - Main application entrypoint for      */
/*    tux64-mkrom.                                                            */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"

#include "tux64-mkrom/arguments.h"
#include <tux64/log.h>

enum Tux64MkromLogLevel {
   TUX64_MKROM_LOG_LEVEL_INFO,
   TUX64_MKROM_LOG_LEVEL_WARNING,
   TUX64_MKROM_LOG_LEVEL_ERROR
};

enum Tux64MkromExitStatus {
   TUX64_MKROM_EXIT_STATUS_OK = 0u,
   TUX64_MKROM_EXIT_STATUS_INVALID_ARGUMENTS
};

struct Tux64MkromExitPayloadInvalidArguments {
   struct Tux64MkromArgumentsParseResult info;
};

union Tux64MkromExitPayload {
   struct Tux64MkromExitPayloadInvalidArguments invalid_arguments;
};

struct Tux64MkromExitResult {
   enum Tux64MkromExitStatus status;
   union Tux64MkromExitPayload payload;
};

static void
tux64_mkrom_exit_result_display_invalid_arguments_invalid(
   const struct Tux64MkromArgumentsParsePayloadInvalid * self
) {
   if (self->parameter == TUX64_NULLPTR) {
      TUX64_LOG_ERROR_FMT(
         "invalid argument \'%s\'",
         self->argument
      );
      return;
   }

   TUX64_LOG_ERROR_FMT(
      "invalid parameter \'%s\' for argument \'%s\'",
      self->parameter,
      self->argument
   );

   return;
}

static void
tux64_mkrom_exit_result_display_invalid_arguments(
   const struct Tux64MkromExitPayloadInvalidArguments * self      
) {
   switch (self->info.status) {
      case TUX64_MKROM_ARGUMENTS_PARSE_STATUS_OK:
      case TUX64_MKROM_ARGUMENTS_PARSE_STATUS_EXIT:
         TUX64_UNREACHABLE;
      case TUX64_MKROM_ARGUMENTS_PARSE_STATUS_OUT_OF_MEMORY:
        TUX64_LOG_ERROR("arguments parser ran out of memory");
         break;
      case TUX64_MKROM_ARGUMENTS_PARSE_STATUS_INVALID:
         tux64_mkrom_exit_result_display_invalid_arguments_invalid(&self->info.payload.invalid);
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
      case TUX64_MKROM_EXIT_STATUS_INVALID_ARGUMENTS:
         tux64_mkrom_exit_result_display_invalid_arguments(&self->payload.invalid_arguments);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static struct Tux64MkromExitResult
tux64_mkrom_main(int argc, char ** argv) {
   struct Tux64MkromExitResult result;
   struct Tux64MkromArgumentsParseResult args_parse_result;
   struct Tux64MkromArguments args;

   args_parse_result = tux64_mkrom_arguments_parse(argc, argv, &args);
   switch (args_parse_result.status) {
      case TUX64_MKROM_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_MKROM_ARGUMENTS_PARSE_STATUS_EXIT:
         result.status = TUX64_MKROM_EXIT_STATUS_OK;
         return result;
      default:
         result.status = TUX64_MKROM_EXIT_STATUS_INVALID_ARGUMENTS;
         result.payload.invalid_arguments.info = args_parse_result;
         return result;
   }

   /* TODO: implement */
   (void)args;

   result.status = TUX64_MKROM_EXIT_STATUS_OK;
   return result;
}

int main(int argc, char ** argv) {
   struct Tux64MkromExitResult exit_result;

   exit_result = tux64_mkrom_main(argc, argv);

   tux64_mkrom_exit_result_display(&exit_result);
   return (int)exit_result.status;
}

