/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/sync.h - Header for synchronization primitives. */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_SYNC_H
#define _TUX64_BOOT_STAGE1_SYNC_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

struct Tux64BootStage1SyncSpinlock {
   volatile Tux64Boolean held;
};

/*----------------------------------------------------------------------------*/
/* Holds the spinlock, waiting for someone to release the spinlock.           */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_sync_spinlock_hold(
   struct Tux64BootStage1SyncSpinlock * spinlock
);
/*----------------------------------------------------------------------------*/
/* Releases any holders of the spinlock.  If nobody is holding the spinlock,  */
/* this does nothing.                                                         */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_sync_spinlock_release(
   struct Tux64BootStage1SyncSpinlock * spinlock
);

/*----------------------------------------------------------------------------*/
/* Checks if the spinlock is currently being held by one or more holders.     */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_sync_spinlock_is_held(
   struct Tux64BootStage1SyncSpinlock * spinlock
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_SYNC_H */

