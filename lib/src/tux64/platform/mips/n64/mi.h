/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/mi.h - MIPS Interface (MI) header.         */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_MI_H
#define _TUX64_PLATFORM_MIPS_N64_MI_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/n64/memory-map.h"

/* addresses of MI registers */
#define TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_MODE\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI +\
      0x00000000u\
   )
#define TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_INTERRUPT\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI +\
      0x00000008u\
   )
#define TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_MASK\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI +\
      0x0000000cu\
   )
#define TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_IQUE_SECURE_EXCEPTION\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI +\
      0x00000014u\
   )
#define TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_IQUE_RANDOM\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI +\
      0x0000002cu\
   )
#define TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_IQUE_INTERRUPT\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI +\
      0x00000038u\
   )
#define TUX64_PLATFORM_MIPS_N64_MI_ADDRESS_IQUE_MASK\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI +\
      0x0000003cu\
   )

/* MI_INTERRUPT bits */
#define TUX64_PLATFORM_MIPS_N64_MI_INTERRUPT_BIT_SP\
   (1u << 0u)
#define TUX64_PLATFORM_MIPS_N64_MI_INTERRUPT_BIT_SI\
   (1u << 1u)
#define TUX64_PLATFORM_MIPS_N64_MI_INTERRUPT_BIT_AI\
   (1u << 2u)
#define TUX64_PLATFORM_MIPS_N64_MI_INTERRUPT_BIT_VI\
   (1u << 3u)
#define TUX64_PLATFORM_MIPS_N64_MI_INTERRUPT_BIT_PI\
   (1u << 4u)
#define TUX64_PLATFORM_MIPS_N64_MI_INTERRUPT_BIT_DP\
   (1u << 5u)

/* MI_MASK bits */
#define TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_SP\
   (1u << 0u)
#define TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_SI\
   (1u << 1u)
#define TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_AI\
   (1u << 2u)
#define TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_VI\
   (1u << 3u)
#define TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_PI\
   (1u << 4u)
#define TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_DP\
   (1u << 5u)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_MI_H */

