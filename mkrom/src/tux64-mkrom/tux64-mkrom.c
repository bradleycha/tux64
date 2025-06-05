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
   TUX64_MKROM_EXIT_STATUS_OK
};

static enum Tux64MkromExitStatus
tux64_mkrom_main(
   const char * const * args,
   Tux64UInt32 args_count
) {
   (void)args;
   (void)args_count;
   (void)fputs("Hello from tux64-mkrom!\n", stdout);
   return TUX64_MKROM_EXIT_STATUS_OK;
}

int main(int argc, char ** argv) {
   enum Tux64MkromExitStatus exit_status;

   exit_status = tux64_mkrom_main(
      (const char * const *)argv,
      (Tux64UInt32)argc
   );

   return (int)exit_status;
}

