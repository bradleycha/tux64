/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/pif.h - RDRAM register and constant        */
/*    definitions.                                                            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_RDRAM_H
#define _TUX64_PLATFORM_MIPS_N64_RDRAM_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/n64/memory-map.h"

/*----------------------------------------------------------------------------*/
/* Defines the offsets for RDRAM registers, relative to the register base.    */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DEVICE_TYPE\
   (0x00u)
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DEVICE_ID\
   (0x04u)
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DELAY\
   (0x08u)
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_MODE\
   (0x0cu)
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_REF_INTERVAL\
   (0x10u)
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_REF_ROW\
   (0x14u)
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_RAS_INTERVAL\
   (0x18u)
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_MIN_INTERVAL\
   (0x1cu)
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_ADDRESS_SELECT\
   (0x20u)
#define TUX64_PLATFORM_MIPS_N64_RDRAM_OFFSET_DEVICE_MANUFACTURER\
   (0x24u)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_RDRAM_H */

