/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/ipl2.h - Definitions for parameters passed from IPL2.  */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_IPL2_H
#define _TUX64_BOOT_IPL2_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

enum Tux64BootIpl2RomType {
   TUX64_BOOT_IPL2_ROM_TYPE_CART = 0u,
   TUX64_BOOT_IPL2_ROM_TYPE_64DD = 1u
};

enum Tux64BootIpl2VideoStandard {
   TUX64_BOOT_IPL2_VIDEO_STANDARD_PAL  = 0u,
   TUX64_BOOT_IPL2_VIDEO_STANDARD_NTSC = 1u,
   TUX64_BOOT_IPL2_VIDEO_STANDARD_MPAL = 2u
};

enum Tux64BootIpl2ResetType {
   TUX64_BOOT_IPL2_RESET_TYPE_HARD = 0u,
   TUX64_BOOT_IPL2_RESET_TYPE_SOFT = 1u
};

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_IPL2_H */

