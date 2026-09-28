/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* init/src/tux64-init/mount.c - Implementations for filesystem mounting.     */
/*----------------------------------------------------------------------------*/

#include "tux64-init/tux64-init.h"
#include "tux64-init/mount.h"

#include <tux64/log.h>

#include <sys/mount.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

struct Tux64InitMountPseudoFilesystem {
   const char * name;
   const char * mountpoint;
   const char * type;
};

static const struct Tux64InitMountPseudoFilesystem
tux64_init_mount_pseudo_filesystem_list[] = {
   {
      .name       = "devfs",
      .mountpoint = "/dev",
      .type       = "devtmpfs"
   }
#if TUX64_INIT_CONFIG_PROCFS
   ,{
      .name       = "procfs",
      .mountpoint = "/proc",
      .type       = "proc"
   }
#endif /* TUX64_INIT_CONFIG_PROCFS */
#if TUX64_INIT_CONFIG_SYSFS
   ,{
      .name       = "sysfs",
      .mountpoint = "/sys",
      .type       = "sysfs"
   }
#endif /* TUX64_INIT_CONFIG_SYSFS */
};

#define TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_LIST_COUNT \
   TUX64_ARRAY_ELEMENTS(tux64_init_mount_pseudo_filesystem_list)

static void
tux64_init_mount_pseudo_filesystem(
   const struct Tux64InitMountPseudoFilesystem * fs
) {
   int status;

   status = mount(
      fs->mountpoint,
      fs->mountpoint,
      fs->type,
      0u,
      NULL
   );

   if (status == -1) {
      TUX64_LOG_ERROR_FMT(
         "failed to mount %s to %s (%s)",
         fs->name,
         fs->mountpoint,
         strerror(errno)
      );
   }

   /* for now, there's nothing more to do.  yippee! */
   return;
}

void
tux64_init_mount_pseudo_filesystems(void) {
   const struct Tux64InitMountPseudoFilesystem * fs;
   Tux64UInt32 i;

   fs = tux64_init_mount_pseudo_filesystem_list;
   i  = TUX64_LITERAL_UINT8(TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_LIST_COUNT);
   do {
      tux64_init_mount_pseudo_filesystem(fs);
      fs++;
      i--;
   } while (i != TUX64_LITERAL_UINT8(0u));

   return;
}

