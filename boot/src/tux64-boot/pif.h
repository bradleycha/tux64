/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/pif.h - Header for PIF interfaces.                     */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_PIF_H
#define _TUX64_BOOT_PIF_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Sends the boot terminate command to the PIF.  This should be called once   */
/* at the start of the boot process to prevent the console from locking up    */
/* after 5 seconds without boot termination.                                  */
/*----------------------------------------------------------------------------*/
void
tux64_boot_pif_terminate_boot(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_PIF_H */

