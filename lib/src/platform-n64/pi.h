/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/platform-n64/pi.h - Peripheral Interface (PI) header.              */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_N64_PI_H
#define _TUX64_PLATFORM_N64_PI_H
/*------------------------------------------------------------------------------*/

#include "tux64.h"
#include "platform_n64/memory_map.h"

/* addresses of PI registers */
#define TUX64_PLATFORM_N64_PI_ADDRESS_DRAM_ADDR\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000000u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_CART_ADDR\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000004u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_RD_LEN\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000008u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_WR_LEN\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x0000000cu\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_STATUS\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000010u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_BSD_DOM1_LAT\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000014u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_BSD_DOM1_PWD\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000018u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_BSD_DOM1_PGS\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x0000001cu\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_BSD_DOM1_RLS\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000020u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_BSD_DOM2_LAT\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000024u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_BSD_DOM2_PWD\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000028u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_BSD_DOM2_PGS\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x0000002cu\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_BSD_DOM2_RLS\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000030u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_IQUE_ATB_UPPER\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000040u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_IQUE_NAND_CTRL\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000048u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_IQUE_NAND_CFG\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x0000004cu\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_IQUE_RD_LEN\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000058u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_IQUE_WR_LEN\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x0000005cu\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_IQUE_GPIO\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000060u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_IQUE_NAND_ADDR\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00000070u\
   )
#define TUX64_PLATFORM_N64_PI_ADDRESS_IQUE_ATB_LOWER\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_PI +\
      0x00010500u\
   )

/* number of PI register table entries */
#define TUX64_PLATFORM_N64_PI_BSD_DOM_COUNT\
   (2u)
#define TUX64_PLATFORM_N64_PI_IQUE_ATB_LOWER_COUNT\
   (192u)

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_N64_PI_H */

