/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/vr4300/vr4300.h - Header for general VR4300    */
/*    constants and definitions.                                              */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_VR4300_H
#define _TUX64_PLATFORM_MIPS_VR4300_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/* whether to enable VR4300-specific code or not */
#define TUX64_PLATFORM_MIPS_VR4300_ENABLE\
   (TUX64_CONFIG_PLATFORM_MIPS_VR4300 && TUX64_CONFIG_PLATFORM_CPU_ISA_MIPS)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_VR4300_H */

