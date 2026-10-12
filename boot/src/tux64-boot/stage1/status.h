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
   TUX64_BOOT_STAGE1_STATUS_CODE_CHOOSE_CONSOLE_TYPE                 = 'C',
   TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_MEMORY                   = 'D',
   TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_RNG                      = 'E',
   TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_VIDEO                    = 'F',
   TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_AUDIO                    = 'G',
   TUX64_BOOT_STAGE1_STATUS_CODE_PIF_TERMINATE_BOOT                  = 'H',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START                    = 'I',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_KERNEL         = 'J',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_INITRAMFS      = 'K',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_COMMAND_LINE   = 'L',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START_KERNEL             = 'M',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_STAGE2         = 'N',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START_STAGE2             = 'O'
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

