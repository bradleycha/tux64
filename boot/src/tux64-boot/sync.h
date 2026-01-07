/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/sync.h - Header for synchronization primitives.        */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_SYNC_H
#define _TUX64_BOOT_SYNC_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

struct Tux64BootSyncFence {
   volatile Tux64Boolean flag;
};

/*----------------------------------------------------------------------------*/
/* Initializes the fence to be unsignaled.                                    */
/*----------------------------------------------------------------------------*/
void
tux64_boot_sync_fence_initialize(
   struct Tux64BootSyncFence * fence
);

/*----------------------------------------------------------------------------*/
/* Waits in a low-power spinlock until the fence is signaled.                 */
/*----------------------------------------------------------------------------*/
void
tux64_boot_sync_fence_wait(
   struct Tux64BootSyncFence * fence
);

/*----------------------------------------------------------------------------*/
/* Signals the fence, releasing any code waiting on it.                       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_sync_fence_signal(
   struct Tux64BootSyncFence * fence
);

/*----------------------------------------------------------------------------*/
/* Returns if the fence has been signaled or not.                             */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_sync_fence_is_signaled(
   struct Tux64BootSyncFence * fence
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_SYNC_H */

