/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/status.h - Stage-1 status code inferface.       */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_STATUS_H
#define _TUX64_BOOT_STAGE1_STATUS_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

enum Tux64BootStage1StatusCode {
   TUX64_BOOT_STAGE1_STATUS_CODE_BEGIN                = 'A',
   TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_INTERRUPT = 'B',
   TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_VIDEO     = 'C',
   TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_LOOP            = 'D'
};

/*----------------------------------------------------------------------------*/
/* Initializes the status struct.  Should be called before writing status     */
/* codes.                                                                     */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_status_initialize(void);

/*----------------------------------------------------------------------------*/
/* Writes the given status code to the status struct.                         */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_status_code_write(
   enum Tux64BootStage1StatusCode code
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_STATUS_H */

