/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/status.h - Stage-2 status code inferface.       */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE2_STATUS_H
#define _TUX64_BOOT_STAGE2_STATUS_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

enum Tux64BootStage2StatusCode {
   TUX64_BOOT_STAGE2_STATUS_CODE_BEGIN                   = 'A',
   TUX64_BOOT_STAGE2_STATUS_CODE_LOAD_FILE_KERNEL        = 'B',
   TUX64_BOOT_STAGE2_STATUS_CODE_LOAD_FILE_INITRAMFS     = 'C',
   TUX64_BOOT_STAGE2_STATUS_CODE_LOAD_FILE_COMMAND_LINE  = 'D',
   TUX64_BOOT_STAGE2_STATUS_CODE_START_KERNEL            = 'E'
};

/*----------------------------------------------------------------------------*/
/* Writes the given status code to the status struct.  This should be called  */
/* after initializing the status code with                                    */
/* tux64_boot_status_code_initialize().                                       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage2_status_code_write(
   enum Tux64BootStage2StatusCode code
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_STATUS_H */

