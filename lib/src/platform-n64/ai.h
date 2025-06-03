/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/platform-n64/ai.h - Audio Interface (AI) header.                   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_N64_AI_H
#define _TUX64_PLATFORM_N64_AI_H
/*------------------------------------------------------------------------------*/

#include "tux64.h"
#include "platform_n64/memory_map.h"

/* ...hey look guys, we have AI on the N64!  now please fund my AI startup */
/* which specializes in real-time AI-generation of sound! ho, ho, ho! */

/* addresses of AI registers */
#define TUX64_PLATFORM_N64_AI_ADDRESS_DRAM_ADDR\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +\
      0x00000000u\
   )
#define TUX64_PLATFORM_N64_AI_ADDRESS_LENGTH\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +\
      0x00000004u\
   )
#define TUX64_PLATFORM_N64_AI_ADDRESS_CONTROL\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +\
      0x00000008u\
   )
#define TUX64_PLATFORM_N64_AI_ADDRESS_STATUS\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +\
      0x0000000cu\
   )
#define TUX64_PLATFORM_N64_AI_ADDRESS_DAC_RATE\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +\
      0x00000010u\
   )
#define TUX64_PLATFORM_N64_AI_ADDRESS_BIT_RATE\
   (\
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +\
      0x00000014u\
   )

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_N64_AI_H */

