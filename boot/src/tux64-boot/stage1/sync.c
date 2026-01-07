/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/sync.c - Implementations for synchronization    */
/*    primitives.                                                             */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/sync.h"

#include "tux64-boot/stage1/idle.h"
#include "tux64-boot/stage1/video.h"

Tux64Boolean
tux64_boot_stage1_sync_preemption_requested(void) {
   if (tux64_boot_stage1_video_vblank_triggered() == TUX64_BOOLEAN_TRUE) {
      return TUX64_BOOLEAN_TRUE;
   }

   return TUX64_BOOLEAN_FALSE;
}

void
tux64_boot_stage1_sync_preemption_wait(void) {
   tux64_boot_stage1_idle_enter();
   while (tux64_boot_stage1_sync_preemption_requested() == TUX64_BOOLEAN_FALSE) {}
   tux64_boot_stage1_idle_exit();

   return;
}

void
tux64_boot_stage1_sync_fence_initialize(
   struct Tux64BootStage1SyncFence * fence
) {
   fence->flag = TUX64_BOOLEAN_FALSE;
   return;
}

void
tux64_boot_stage1_sync_fence_wait(
   struct Tux64BootStage1SyncFence * fence
) {
   tux64_boot_stage1_idle_enter();
   while (fence->flag == TUX64_BOOLEAN_FALSE) {}
   tux64_boot_stage1_idle_exit();

   return;
}

void
tux64_boot_stage1_sync_fence_signal(
   struct Tux64BootStage1SyncFence * fence
) {
   fence->flag = TUX64_BOOLEAN_TRUE;
   return;
}

Tux64Boolean
tux64_boot_stage1_sync_fence_is_signaled(
   struct Tux64BootStage1SyncFence * fence
) {
   return fence->flag;
}

