/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* init/src/tux64-init/tux64-init.c - Main application entrypoint for         */
/*    tux64-init.                                                             */
/*----------------------------------------------------------------------------*/

#include "tux64-init/tux64-init.h"
#include "tux64-init/system-info.h"

#include <tux64/log.h>

#include <unistd.h>
#include <limits.h>

static void
tux64_init_main(void) {
   TUX64_LOG_INFO(TUX64_INIT_PACKAGE_NAME " version " TUX64_INIT_PACKAGE_VERSION);

   if (TUX64_INIT_CONFIG_STARTUP_SYSTEM_INFO) {
      tux64_init_system_info_print();
   }

   /* TODO: implement actual init system.  at the very least, we need to do   */
   /* the following:                                                          */
   /*                                                                         */
   /* - mount pseudo filesystems (/dev, /proc, /sys)                          */
   /* - read fstab, if present, and mount filesystem in there                 */
   /* - implement a configuration script to set startup processes             */
   /* - start startup processes                                               */
   /* - drop the user to a shell of their choice                              */
   TUX64_LOG_WARNING("TODO: implement rest of init");
   return;
}

int main(void) {
   tux64_init_main();

   /* init should never terminate, so we just permanently idle. */
   while (TUX64_BOOLEAN_TRUE) sleep(INT_MAX);
   TUX64_UNREACHABLE;
}

