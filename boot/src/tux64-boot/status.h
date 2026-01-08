/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/status.h - Boot status code header.                    */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STATUS_H
#define _TUX64_BOOT_STATUS_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Initializes the low half of the status code word, which is static and      */
/* doesn't change based on boot status.                                       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_status_code_initialize(void);

/*----------------------------------------------------------------------------*/
/* Formats and writes the current boot status code high half.  The "stage"    */
/* and "code" should be the ASCII representation of the stage and code.       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_status_code_write(
   char stage,
   char code
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STATUS_H */

