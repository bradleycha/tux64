/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/vi.h - Video Interface (VI) header.        */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_VI_H
#define _TUX64_PLATFORM_MIPS_N64_VI_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/n64/memory-map.h"

/* addresses of VI registers */
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_CTRL\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000000u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_ORIGIN\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000004u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_WIDTH\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000008u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_V_INTR\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x0000000cu\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_V_CURRENT\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000010u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_BURST\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000014u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_V_TOTAL\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000018u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_H_TOTAL\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x0000001cu\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_H_TOTAL_LEAP\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000020u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_H_VIDEO\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000024u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_V_VIDEO\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000028u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_V_BURST\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x0000002cu\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_X_SCALE\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000030u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_Y_SCALE\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000034u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_TEST_ADDR\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000038u\
   )
#define TUX64_PLATFORM_MIPS_N64_VI_ADDRESS_STAGED_DATA\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI +\
      0x0000003cu\
   )

/* bit flags, masks, and what-not relating to VI registers */
#define TUX64_PLATFORM_MIPS_N64_VI_TYPE_MASK\
   (0x00000003u)

#define TUX64_PLATFORM_MIPS_N64_VI_DEDITHER_BIT_OFFSET\
   (16u)

enum Tux64PlatformMipsN64ViDedither {
   TUX64_PLATFORM_MIPS_N64_VI_DEDITHER_DISABLE  = 0x0u << TUX64_PLATFORM_MIPS_N64_VI_DEDITHER_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_DEDITHER_ENABLE   = 0x1u << TUX64_PLATFORM_MIPS_N64_VI_DEDITHER_BIT_OFFSET
};

#define TUX64_PLATFORM_MIPS_N64_VI_PIXEL_ADVANCE_BIT_OFFSET\
   (12u)

#define TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_BIT_OFFSET\
   (8u)

enum Tux64PlatformMipsN64ViAaMode {
   TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_ENABLE_FETCH_ALL         = 0x0u << TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_ENABLE_FETCH_AS_NEEDED   = 0x1u << TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_ONLY_RESAMPLING          = 0x2u << TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_DISABLE                  = 0x3u << TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_BIT_OFFSET
};

#define TUX64_PLATFORM_MIPS_N64_VI_SERRATE_BIT_OFFSET\
   (6u)

enum Tux64PlatformMipsN64ViSerrate {
   TUX64_PLATFORM_MIPS_N64_VI_SERRATE_DISABLE   = 0x0u << TUX64_PLATFORM_MIPS_N64_VI_SERRATE_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_SERRATE_ENABLE    = 0x1u << TUX64_PLATFORM_MIPS_N64_VI_SERRATE_BIT_OFFSET
};

#define TUX64_PLATFORM_MIPS_N64_VI_DIVOT_BIT_OFFSET\
   (4u)

enum Tux64PlatformMipsN64ViDivot {
   TUX64_PLATFORM_MIPS_N64_VI_DIVOT_DISABLE  = 0x0u << TUX64_PLATFORM_MIPS_N64_VI_DIVOT_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_DIVOT_ENABLE   = 0x1u << TUX64_PLATFORM_MIPS_N64_VI_DIVOT_BIT_OFFSET
};

#define TUX64_PLATFORM_MIPS_N64_VI_GAMMA_BIT_OFFSET\
   (3u)

enum Tux64PlatformMipsN64ViGamma {
   TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DISABLE  = 0x0u << TUX64_PLATFORM_MIPS_N64_VI_GAMMA_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_GAMMA_ENABLE   = 0x1u << TUX64_PLATFORM_MIPS_N64_VI_GAMMA_BIT_OFFSET
};

#define TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DITHER_BIT_OFFSET\
   (2u)

enum Tux64PlatformMipsN64ViGammaDither {
   TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DITHER_DISABLE = 0x0u << TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DITHER_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DITHER_ENABLE  = 0x1u << TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DITHER_BIT_OFFSET
};

#define TUX64_PLATFORM_MIPS_N64_VI_TYPE_BIT_OFFSET\
   (0u)

enum Tux64PlatformMipsN64ViType {
   TUX64_PLATFORM_MIPS_N64_VI_TYPE_DISABLED = 0x0u << TUX64_PLATFORM_MIPS_N64_VI_TYPE_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_TYPE_RGBA5553 = 0x2u << TUX64_PLATFORM_MIPS_N64_VI_TYPE_BIT_OFFSET,
   TUX64_PLATFORM_MIPS_N64_VI_TYPE_RGBA8888 = 0x3u << TUX64_PLATFORM_MIPS_N64_VI_TYPE_BIT_OFFSET
};

#define TUX64_PLATFORM_MIPS_N64_VI_LEAP_BIT_OFFSET\
   (16u)

#define TUX64_PLATFORM_MIPS_N64_VI_LEAP_DEFAULT\
   (0x15u << TUX64_PLATFORM_MIPS_N64_VI_LEAP_BIT_OFFSET)

#define TUX64_PLATFORM_MIPS_N64_VI_H_START_BIT_OFFSET\
   (16u)
#define TUX64_PLATFORM_MIPS_N64_VI_H_END_BIT_OFFSET\
   (0u)

#define TUX64_PLATFORM_MIPS_N64_VI_V_START_BIT_OFFSET\
   (16u)
#define TUX64_PLATFORM_MIPS_N64_VI_V_END_BIT_OFFSET\
   (0u)

#define TUX64_PLATFORM_MIPS_N64_VI_V_BURST_START_BIT_OFFSET\
   (16u)
#define TUX64_PLATFORM_MIPS_N64_VI_V_BURST_END_BIT_OFFSET\
   (0u)

#define TUX64_PLATFORM_MIPS_N64_VI_X_OFFSET_BIT_OFFSET\
   (16u)
#define TUX64_PLATFORM_MIPS_N64_VI_X_SCALE_BIT_OFFSET\
   (0u)
#define TUX64_PLATFORM_MIPS_N64_VI_Y_OFFSET_BIT_OFFSET\
   (16u)
#define TUX64_PLATFORM_MIPS_N64_VI_Y_SCALE_BIT_OFFSET\
   (0u)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_VI_H */

