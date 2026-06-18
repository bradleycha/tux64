/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/pif.h - PIF definitions.                   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_PIF_H
#define _TUX64_PLATFORM_MIPS_N64_PIF_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/*----------------------------------------------------------------------------*/
/* Bit flags which send various commands to the PIF.                          */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_N64_PIF_COMMAND_CONFIGURE_JOYBUS_FRAME\
   (1u << 0u)
#define TUX64_PLATFORM_MIPS_N64_PIF_COMMAND_CHALLENGE_RESPONSE_PROTECTION\
   (1u << 1u)
#define TUX64_PLATFORM_MIPS_N64_PIF_COMMAND_JOYBUS_FLAG_BIT_0\
   (1u << 2u)
#define TUX64_PLATFORM_MIPS_N64_PIF_COMMAND_JOYBUS_FLAG_BIT_1\
   (1u << 3u)
#define TUX64_PLATFORM_MIPS_N64_PIF_COMMAND_TERMINATE_BOOT\
   (1u << 3u)
#define TUX64_PLATFORM_MIPS_N64_PIF_COMMAND_ROM_LOCKOUT\
   (1u << 4u)
#define TUX64_PLATFORM_MIPS_N64_PIF_COMMAND_ACQUIRE_CHECKSUM\
   (1u << 5u)
#define TUX64_PLATFORM_MIPS_N64_PIF_COMMAND_RUN_CHECKSUM\
   (1u << 6u)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_PIF_H */

