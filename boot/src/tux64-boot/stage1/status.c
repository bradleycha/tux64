/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/status.c - Stage-1 status code implementation.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/status.h"

#include "tux64-boot/status.h"

void
tux64_boot_stage1_status_code_write(
   enum Tux64BootStage1StatusCode code
) {
   tux64_boot_status_code_write('1', (char)code);
   return;
}

