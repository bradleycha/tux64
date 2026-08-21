/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/flag.h - Header for checking boot flags.               */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_FLAG_H
#define _TUX64_BOOT_FLAG_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Checks if various boot flags are enable.  These depend both on the boot    */
/* header flag bits, as well as the build configuration.                      */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_flag_checksum(void);
Tux64Boolean
tux64_boot_flag_delay(void);
Tux64Boolean
tux64_boot_flag_memory_display(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_FLAG_H */

