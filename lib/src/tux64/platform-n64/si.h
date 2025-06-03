/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform-n64/si.h - Serial Interface (SI) header.            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_N64_SI_H
#define _TUX64_PLATFORM_N64_SI_H
/*------------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform-n64/memory-map.h"

/* addresses of SI registers */
#define TUX64_PLATFORM_N64_SI_ADDRESS_DRAM_ADDR\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +\
      0x00000000u\
   )
#define TUX64_PLATFORM_N64_SI_ADDRESS_PIF_AD_RD64B\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +\
      0x00000004u\
   )
#define TUX64_PLATFORM_N64_SI_ADDRESS_PIF_AD_WR4B\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +\
      0x00000008u\
   )
#define TUX64_PLATFORM_N64_SI_ADDRESS_PIF_AD_WR64B\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +\
      0x00000010u\
   )
#define TUX64_PLATFORM_N64_SI_ADDRESS_PIF_AD_RD4B\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +\
      0x00000014u\
   )
#define TUX64_PLATFORM_N64_SI_ADDRESS_STATUS\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +\
      0x00000018u\
   )

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_N64_SI_H */

