/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.c - Stage-2 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/prelude.h"
#include "tux64-boot/halt.h"

#include "tux64-boot/stage2/status.h"

void
tux64_boot_stage2_main(void) {
   tux64_boot_prelude();

   tux64_boot_stage2_status_code_write(TUX64_BOOT_STAGE2_STATUS_CODE_BEGIN);

   /* TODO: implement stage-2 loader ;) */
   tux64_boot_halt();
   TUX64_UNREACHABLE;
}

