/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/preempt.c - Implementations for preemption      */
/*    management.                                                             */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/preempt.h"

#include "tux64-boot/idle.h"
#include "tux64-boot/stage1/video.h"

Tux64Boolean
tux64_boot_stage1_preempt_yield(void) {
   if (tux64_boot_stage1_video_vblank_triggered() == TUX64_BOOLEAN_TRUE) {
      return TUX64_BOOLEAN_TRUE;
   }

   return TUX64_BOOLEAN_FALSE;
}

void
tux64_boot_stage1_preempt_wait(void) {
   tux64_boot_idle_enter();
   while (tux64_boot_stage1_preempt_yield() == TUX64_BOOLEAN_FALSE) {}
   tux64_boot_idle_exit();

   return;
}

