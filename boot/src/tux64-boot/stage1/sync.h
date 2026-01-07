/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/sync.h - Header for synchronization primitives. */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_SYNC_H
#define _TUX64_BOOT_STAGE1_SYNC_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Whether the CPU should be preempted under heavy load where it may not      */
/* return for a long period of time, most commonly due to I/O.                */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_sync_preemption_requested(void);

/*----------------------------------------------------------------------------*/
/* Idle the CPU until preemption is requested.                                */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_sync_preemption_wait(void);

struct Tux64BootStage1SyncFence {
   volatile Tux64Boolean flag;
};

/*----------------------------------------------------------------------------*/
/* Initializes the fence to be unsignaled.                                    */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_sync_fence_initialize(
   struct Tux64BootStage1SyncFence * fence
);

/*----------------------------------------------------------------------------*/
/* Waits in a low-power spinlock until the fence is signaled.                 */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_sync_fence_wait(
   struct Tux64BootStage1SyncFence * fence
);

/*----------------------------------------------------------------------------*/
/* Signals the fence, releasing any code waiting on it.                       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_sync_fence_signal(
   struct Tux64BootStage1SyncFence * fence
);

/*----------------------------------------------------------------------------*/
/* Returns if the fence has been signaled or not.                             */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_sync_fence_is_signaled(
   struct Tux64BootStage1SyncFence * fence
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_SYNC_H */

