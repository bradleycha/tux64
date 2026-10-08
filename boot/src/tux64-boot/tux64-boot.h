/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
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
#error "tux64-lib has TUX64_CONFIG_PLATFORM_MIPS_N64 disabled.  please rebuild tux64-lib with --enable-platform-mips-n64."
#endif /* !TUX64_CONFIG_PLATFORM_MIPS_N64 */

#if !TUX64_CONFIG_PLATFORM_MIPS_VR4300
#error "tux64-lib has TUX64_CONFIG_PLATFORM_MIPS_VR4300 disabled.  please rebuild tux64-lib with --enable-platform-mips-vr4300."
#endif /* !TUX64_CONFIG_PLATFORM_MIPS_VR4300 */

#if TUX64_ENDIAN_FORMAT_NATIVE != TUX64_ENDIAN_FORMAT_BIG
#error "only big-endian bootloaders are currently supported.  please ensure your build configuration targets a big-endian system."
#endif /* TUX64_ENDIAN_FORMAT_NATIVE != TUX64_ENDIAN_FORMAT_BIG */

#if !TUX64_PREPROCESSOR_ONLY
/*----------------------------------------------------------------------------*/

#define TUX64_BOOT_CONSOLE_TYPE_UNKNOWN \
   !( \
      TUX64_BOOT_CONFIG_CONSOLE_TYPE_PAL || \
      TUX64_BOOT_CONFIG_CONSOLE_TYPE_NTSC || \
      TUX64_BOOT_CONFIG_CONSOLE_TYPE_MPAL || \
      TUX64_BOOT_CONFIG_CONSOLE_TYPE_IQUE \
   )

/* we have conditionally compiled code which breaks if we have no console */
/* types enabled.  this should be blocked in the configure script, but just */
/* in case it doesn't, we have this extra layer of protection. */
#if TUX64_BOOT_CONSOLE_TYPE_UNKNOWN
#error all console types are disabled.  please reconfigure with at least one console type using --enable-console-type-[type]
#endif /* TUX64_BOOT_CONSOLE_TYPE_UNKNOWN */

enum Tux64BootConsoleType {
   /* manually define for compatability with IPL2 video standard enum */
   TUX64_BOOT_CONSOLE_TYPE_PAL   = 0u,
   TUX64_BOOT_CONSOLE_TYPE_NTSC  = 1u,
   TUX64_BOOT_CONSOLE_TYPE_MPAL  = 2u,
   TUX64_BOOT_CONSOLE_TYPE_IQUE  = 3u
};

/*----------------------------------------------------------------------------*/
#endif /* !TUX64_PREPROCESSOR_ONLY */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_H */

