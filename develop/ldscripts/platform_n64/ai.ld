/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* develop/ldscripts/platform_n64/ai.ld - Audio Interface (AI) linker script. */
/*----------------------------------------------------------------------------*/

/* addresses of AI MMIO registers */
TUX64_PLATFORM_N64_AI_MMIO_DRAM_ADDR =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +
      0x00000000
   );
TUX64_PLATFORM_N64_AI_MMIO_LENGTH =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +
      0x00000004
   );
TUX64_PLATFORM_N64_AI_MMIO_CONTROL =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +
      0x00000008
   );
TUX64_PLATFORM_N64_AI_MMIO_STATUS =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +
      0x0000000c
   );
TUX64_PLATFORM_N64_AI_MMIO_DACRATE =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +
      0x00000010
   );
TUX64_PLATFORM_N64_AI_MMIO_BITRATE =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_AI +
      0x00000014
   );

