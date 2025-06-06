/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/tux64-mkrom.c - Main application entrypoint for      */
/*    tux64-mkrom.                                                            */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"

#include <stdio.h>
#include <argp.h>

enum Tux64MkromExitStatus {
   TUX64_MKROM_EXIT_STATUS_OK,
   TUX64_MKROM_EXIT_STATUS_INVALID_ARGUMENTS
};

struct Tux64MkromExitPayloadInvalidArguments {
   error_t reason;
};

union Tux64MkromExitPayload {
   struct Tux64MkromExitPayloadInvalidArguments invalid_arguments;
};

struct Tux64MkromExitResult {
   enum Tux64MkromExitStatus status;
   union Tux64MkromExitPayload payload;
};

static void
tux64_mkrom_exit_result_display_invalid_arguments(
   const struct Tux64MkromExitPayloadInvalidArguments * self
) {
   /* TODO: implement */
   (void)self;
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

   /* TODO: implement */
   (void)argc;
   (void)argv;
   (void)fputs("Hello from tux64-mkrom!\n", stdout);

   result.status = TUX64_MKROM_EXIT_STATUS_OK;
   return result;
}

int main(int argc, char ** argv) {
   struct Tux64MkromExitResult exit_result;

   exit_result = tux64_mkrom_main(argc, argv);

   tux64_mkrom_exit_result_display(&exit_result);
   return (int)exit_result.status;
}

