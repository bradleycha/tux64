/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/tux64-mkrom.c - Main application entrypoint for      */
/*    tux64-mkrom.                                                            */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"

#include <stdio.h>

enum Tux64MkromExitStatus {
   TUX64_MKROM_EXIT_STATUS_OK,
   TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS
};


union Tux64MkromExitPayload {
   Tux64UInt8 temp;
};

struct Tux64MkromExitResult {
   enum Tux64MkromExitStatus status;
   union Tux64MkromExitPayload payload;
};

static void
tux64_mkrom_log_error(
   const char * message
) {
   (void)fprintf(stderr, "error: %s\n", message);
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
         tux64_mkrom_log_error("too many arguments");
         break;
   }

   return;
}

static struct Tux64MkromExitResult
tux64_mkrom_main(
   const char * const * args,
   Tux64UInt8 args_count
) {
   struct Tux64MkromExitResult result;

   /* TODO: implement */
   (void)args;
   (void)args_count;
   (void)fputs("Hello from tux64-mkrom!\n", stdout);

   result.status = TUX64_MKROM_EXIT_STATUS_OK;
   return result;
}

int main(int argc, char ** argv) {
   struct Tux64MkromExitResult exit_result;

   if (argc > TUX64_UINT8_MAX) {
      exit_result.status = TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS;
      goto exit;
   }

   exit_result = tux64_mkrom_main(
      (const char * const *)argv,
      (Tux64UInt8)argc
   );

exit:
   tux64_mkrom_exit_result_display(&exit_result);
   return (int)exit_result.status;
}

