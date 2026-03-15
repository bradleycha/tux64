/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.c - Stage-2 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/halt.h"

void
tux64_boot_stage2_main(void) {
   /* TODO: implement stage-2 loader ;) */
   tux64_boot_halt();
   TUX64_UNREACHABLE;
}

