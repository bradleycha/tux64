/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/start.c - Stage-2 bootloader startup code.      */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/prelude.h"
#include "tux64-boot/stage2/stage2.h"

void
tux64_boot_stage2_start(void)
__attribute__((noreturn, section(".start"), externally_visible));

void
tux64_boot_stage2_start(void) {
   /* in the future, we'll do a similar thing to stage-1 with the register    */
   /* variables in order to ensure a stable ABI across boot stages.           */

   tux64_boot_prelude();

   tux64_boot_stage2_main();

   TUX64_UNREACHABLE;
}

