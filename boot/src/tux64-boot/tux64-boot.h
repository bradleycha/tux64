/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/tux64-boot.h - The project-wide header for tux64-boot. */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_H
#define _TUX64_BOOT_H
/*----------------------------------------------------------------------------*/

#include <tux64/tux64.h>
#include "tux64-boot/config.h"

#if !TUX64_CONFIG_PLATFORM_MIPS_N64
#error "tux64-lib has TUX64_CONFIG_PLATFORM_MIPS_N64 disabled.  please rebuild tux64-lib with --enable-platform-n64."
#endif /* !TUX64_CONFIG_PLATFORM_MIPS_N64 */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_H */

