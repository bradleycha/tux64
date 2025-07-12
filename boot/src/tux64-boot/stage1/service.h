/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/service.h - C bindings for low-level interrupt  */
/*    servicing function.                                                     */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_SERVICE_H
#define _TUX64_BOOT_STAGE1_SERVICE_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* The low-level exception servicing code which is executed by the CPU.       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_service_exception(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_SERVICE_H */

