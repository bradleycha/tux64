/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* init/src/tux64-init/system-info.c - Implementations for system info        */
/*    querying.                                                               */
/*----------------------------------------------------------------------------*/

#include "tux64-init/tux64-init.h"
#include "tux64-init/system-info.h"

#include <tux64/log.h>

#include <inttypes.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>

static void
tux64_init_system_info_print_kernel(void) {
   struct utsname kernel;

   /* uname() is only documented to error with EFAULT if 'buf' is not valid. */
   /* since we know 'buf' is always valid, we can safely ignore errors here. */
   (void)uname(&kernel);

   TUX64_LOG_INFO_FMT(
      "Running on %s/%s version %s",
      kernel.sysname,
      kernel.machine,
      kernel.release
   );

   return;
}

static void
tux64_init_system_info_print_memory_statistic(
   const char * name,
   Tux64UInt32 bytes
) {
   Tux64UInt32 mib_hi;
   Tux64UInt32 mib_lo;

   mib_hi = bytes >> TUX64_LITERAL_UINT8(20u);
   mib_lo = (bytes - (mib_hi << TUX64_LITERAL_UINT8(20u))) >> TUX64_LITERAL_UINT8(10u);

   TUX64_LOG_INFO_FMT(
      "%s | %" PRIu32 ".%03" PRIu32 " MiB (%" PRIu32 " bytes)",
      name, mib_hi, mib_lo, bytes
   );

   return;
}

static void
tux64_init_system_info_print_memory(void) {
   struct sysinfo mem;

   /* we can ignore the error here for the same reason as with uname(). */
   (void)sysinfo(&mem);

   tux64_init_system_info_print_memory_statistic("Memory total   ", mem.totalram);
   tux64_init_system_info_print_memory_statistic("Memory free    ", mem.freeram);
   tux64_init_system_info_print_memory_statistic("Memory shared  ", mem.sharedram);
   tux64_init_system_info_print_memory_statistic("Memory buffered", mem.bufferram);

   return;
}

void
tux64_init_system_info_print(void) {
   tux64_init_system_info_print_kernel();
   tux64_init_system_info_print_memory();
   return;
}

