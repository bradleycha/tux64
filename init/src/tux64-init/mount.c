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
#include <sys/stat.h>
#include <unistd.h>
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

#define TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_DEVPTSFS_NAME       "devptsfs"
#define TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_DEVPTSFS_MOUNTPOINT "/dev/pts"

/* this needs to be seperate because we need to create the /dev/pts directory */
/* before we can mount, but we also need to mount /dev first. */
static const struct Tux64InitMountPseudoFilesystem
tux64_init_mount_pseudo_filesystem_devptsfs = {
   .name       = TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_DEVPTSFS_NAME,
   .mountpoint = TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_DEVPTSFS_MOUNTPOINT,
   .type       = "devpts"
};

static Tux64Boolean
tux64_init_mount_pseudo_filesystem(
   const struct Tux64InitMountPseudoFilesystem * fs
) {
   int status;

   status = mount(
      fs->type,
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

      return TUX64_BOOLEAN_FALSE;
   }

   return TUX64_BOOLEAN_TRUE;
}

#if TUX64_INIT_CONFIG_PSEUDO_TERMINALS
/*----------------------------------------------------------------------------*/
static void
tux64_init_mount_pseudo_filesystem_pseudo_terminals(void) {
   int status;
   Tux64Boolean success;

   status = mkdir(TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_DEVPTSFS_MOUNTPOINT, 0);
   if (status == 1) {
      TUX64_LOG_ERROR_FMT(
         "failed to create mount point for "
         TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_DEVPTSFS_NAME
         " (%s)",
         strerror(errno)
      );

      return;
   }

   success = tux64_init_mount_pseudo_filesystem(
      &tux64_init_mount_pseudo_filesystem_devptsfs
   );
   if (success == TUX64_BOOLEAN_FALSE) {
      /* nothing much we can do if this fails. */
      (void)rmdir(TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_DEVPTSFS_MOUNTPOINT);
   }
   return;
}
/*----------------------------------------------------------------------------*/
#else /* TUX64_INIT_CONFIG_PSEUDO_TERMINALS */
/*----------------------------------------------------------------------------*/
static void
tux64_init_mount_pseudo_filesystem_pseudo_terminals(void) {
   return;
}
/*----------------------------------------------------------------------------*/
#endif /* TUX64_INIT_CONFIG_PSEUDO_TERMINALS */

void
tux64_init_mount_pseudo_filesystems(void) {
   const struct Tux64InitMountPseudoFilesystem * fs;
   Tux64UInt32 i;

   fs = tux64_init_mount_pseudo_filesystem_list;
   i  = TUX64_LITERAL_UINT8(TUX64_INIT_MOUNT_PSEUDO_FILESYSTEM_LIST_COUNT);
   do {
      (void)tux64_init_mount_pseudo_filesystem(fs);
      fs++;
      i--;
   } while (i != TUX64_LITERAL_UINT8(0u));

   /* see comment above as to why we need this seperately */
   tux64_init_mount_pseudo_filesystem_pseudo_terminals();

   return;
}

