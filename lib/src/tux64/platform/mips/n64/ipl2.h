/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64-lib/platform/mips/n64/ipl2.h - Definitions for IPL2 boot     */
/*    parameters.                                                             */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_IPL2_H
#define _TUX64_PLATFORM_MIPS_N64_IPL2_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

enum Tux64PlatformMipsN64Ipl2RomType {
   TUX64_PLATFORM_MIPS_N64_IPL2_ROM_TYPE_CART = 0u,
   TUX64_PLATFORM_MIPS_N64_IPL2_ROM_TYPE_64DD = 1u
};

enum Tux64PlatformMipsN64Ipl2VideoStandard {
   TUX64_PLATFORM_MIPS_N64_IPL2_VIDEO_STANDARD_PAL  = 0u,
   TUX64_PLATFORM_MIPS_N64_IPL2_VIDEO_STANDARD_NTSC = 1u,
   TUX64_PLATFORM_MIPS_N64_IPL2_VIDEO_STANDARD_MPAL = 2u
};

enum Tux64PlatformMipsN64Ipl2ResetType {
   TUX64_PLATFORM_MIPS_N64_IPL2_RESET_TYPE_HARD = 0u,
   TUX64_PLATFORM_MIPS_N64_IPL2_RESET_TYPE_SOFT = 1u
};

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_IPL2_H */

