/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/status.h - Stage-1 bootloader status code       */
/*    header.                                                                 */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_STATUS_H
#define _TUX64_BOOT_STAGE1_STATUS_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* This code stores an 8-byte chunk of memory at the end of RSP DMEM which    */
/* when viewed as ASCII text looks like "STAGE1:c", where 'c' is the current  */
/* boot stage.  The point of this code is to provide information useful to    */
/* debug early boot failures by viewing memory directly.                      */
/*----------------------------------------------------------------------------*/

enum Tux64BootStage1StatusCode {
   TUX64_BOOT_STAGE1_STATUS_CODE_BEGIN = '0'
};

void
tux64_boot_stage1_status_initialize(void);

void
tux64_boot_stage1_status_code_set(
   enum Tux64BootStage1StatusCode code
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_STATUS_H */


