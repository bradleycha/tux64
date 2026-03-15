/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/status.c - Stage-2 status code implementation.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage2/status.h"

#include "tux64-boot/status.h"

void
tux64_boot_stage2_status_code_write(
   enum Tux64BootStage2StatusCode code
) {
   tux64_boot_status_code_write('2', (char)code);
   return;
}

