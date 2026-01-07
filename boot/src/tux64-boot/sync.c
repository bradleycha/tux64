/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/sync.c - Implementations for synchronization           */
/*    primitives.                                                             */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/sync.h"

#include "tux64-boot/idle.h"

void
tux64_boot_sync_fence_initialize(
   struct Tux64BootSyncFence * fence
) {
   fence->flag = TUX64_BOOLEAN_FALSE;
   return;
}

void
tux64_boot_sync_fence_wait(
   struct Tux64BootSyncFence * fence
) {
   tux64_boot_idle_enter();
   while (fence->flag == TUX64_BOOLEAN_FALSE) {}
   tux64_boot_idle_exit();

   return;
}

void
tux64_boot_sync_fence_signal(
   struct Tux64BootSyncFence * fence
) {
   fence->flag = TUX64_BOOLEAN_TRUE;
   return;
}

Tux64Boolean
tux64_boot_sync_fence_is_signaled(
   struct Tux64BootSyncFence * fence
) {
   return fence->flag;
}

