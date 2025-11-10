/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/sync.c - Implementations for synchronization    */
/*    primitives.                                                             */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/sync.h"

#include "tux64-boot/stage1/idle.h"

void
tux64_boot_stage1_sync_spinlock_hold(
   struct Tux64BootStage1SyncSpinlock * spinlock
) {
   spinlock->held = TUX64_BOOLEAN_TRUE;

   tux64_boot_stage1_idle_enter();
   while (spinlock->held == TUX64_BOOLEAN_TRUE) {};
   tux64_boot_stage1_idle_exit();

   return;
}

void
tux64_boot_stage1_sync_spinlock_release(
   struct Tux64BootStage1SyncSpinlock * spinlock
) {
   spinlock->held = TUX64_BOOLEAN_FALSE;
   return;
}

Tux64Boolean
tux64_boot_stage1_sync_spinlock_is_held(
   struct Tux64BootStage1SyncSpinlock * spinlock
) {
   return spinlock->held;
}

