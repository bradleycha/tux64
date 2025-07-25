/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/platform/mips/n64/ri.h - RDRAM Interface (RI) header.              */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_RI_H
#define _TUX64_PLATFORM_MIPS_N64_RI_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/n64/memory-map.h"

/* addresses of RI registers */
#define TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_MODE\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI +\
      0x00000000u\
   )
#define TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_CONFIG\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI +\
      0x00000004u\
   )
#define TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_CURRENT_LOAD\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI +\
      0x00000008u\
   )
#define TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_SELECT\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI +\
      0x0000000cu\
   )
#define TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_REFRESH\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI +\
      0x00000010u\
   )
#define TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_LATENCY\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI +\
      0x00000014u\
   )
#define TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_ERROR\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI +\
      0x00000018u\
   )
#define TUX64_PLATFORM_MIPS_N64_RI_ADDRESS_BANK_STATUS\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI +\
      0x0000001cu\
   )

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_RI_H */

