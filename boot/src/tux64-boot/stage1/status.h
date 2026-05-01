/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/status.h - Stage-1 status code inferface.       */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_STATUS_H
#define _TUX64_BOOT_STAGE1_STATUS_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

enum Tux64BootStage1StatusCode {
   TUX64_BOOT_STAGE1_STATUS_CODE_BEGIN                               = 'A',
   TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_INTERRUPT                = 'B',
   TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_MEMORY                   = 'C',
   TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_VIDEO                    = 'D',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START                    = 'E',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_KERNEL         = 'F',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_INITRAMFS      = 'G',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_COMMAND_LINE   = 'H',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START_KERNEL             = 'I',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_STAGE2         = 'J',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START_STAGE2             = 'K'
};

/*----------------------------------------------------------------------------*/
/* Writes the given status code to the status struct.  This should be called  */
/* after initializing the status code with                                    */
/* tux64_boot_status_code_initialize().                                       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_status_code_write(
   enum Tux64BootStage1StatusCode code
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_STATUS_H */

