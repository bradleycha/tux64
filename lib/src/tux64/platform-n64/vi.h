/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/platform-n64/vi.h - Video Interface (VI) header.                   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_N64_VI_H
#define _TUX64_PLATFORM_N64_VI_H
/*------------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform-n64/memory-map.h"

/* addresses of VI registers */
#define TUX64_PLATFORM_N64_VI_ADDRESS_CTRL\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000000u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_ORIGIN\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000004u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_WIDTH\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000008u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_V_INTR\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x0000000cu\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_V_CURRENT\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000010u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_BURST\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000014u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_V_TOTAL\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000018u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_H_TOTAL\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x0000001cu\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_H_TOTAL_LEAP\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000020u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_H_VIDEO\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000024u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_V_VIDEO\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000028u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_V_BURST\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x0000002cu\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_X_SCALE\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000030u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_Y_SCALE\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000034u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_TEST_ADDR\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x00000038u\
   )
#define TUX64_PLATFORM_N64_VI_ADDRESS_STAGED_DATA\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_VI +\
      0x0000003cu\
   )

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_N64_VI_H */

