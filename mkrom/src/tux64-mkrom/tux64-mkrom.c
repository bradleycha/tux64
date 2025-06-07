/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/tux64-mkrom.c - Main application entrypoint for      */
/*    tux64-mkrom.                                                            */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"

#include <tux64/log.h>
#include <inttypes.h>
#include "tux64-mkrom/arguments.h"

#define TUX64_MKROM_ARGC_MAX TUX64_UINT8_MAX

enum Tux64MkromExitStatus {
   TUX64_MKROM_EXIT_STATUS_OK = 0u,
   TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS
};

struct Tux64MkromExitPayloadTooManyArguments {
   int argc_given;
   Tux64UInt8 argc_max;
};

union Tux64MkromExitPayload {
   struct Tux64MkromExitPayloadTooManyArguments too_many_arguments;
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
tux64_mkrom_exit_result_display(
   const struct Tux64MkromExitResult * self
) {
   switch (self->status) {
      case TUX64_MKROM_EXIT_STATUS_OK:
         break;
      case TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS:
         tux64_mkrom_exit_result_display_too_many_arguments(&self->payload.too_many_arguments);
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

   /* TODO: implement */
   (void)argc;
   (void)argv;
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

