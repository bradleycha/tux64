/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* develop/ldscripts/platform_n64/mi.ld - MIPS Interface (MI) linker script.  */
/*----------------------------------------------------------------------------*/

/* addresses of MI MMIO registers */
TUX64_PLATFORM_N64_MI_MMIO_ADDRESS_MODE = 
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_MI +
      0x00000000
   );
TUX64_PLATFORM_N64_MI_MMIO_ADDRESS_INTERRUPT =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_MI +
      0x00000008
   );
TUX64_PLATFORM_N64_MI_MMIO_ADDRESS_MASK =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_MI +
      0x0000000c
   );
TUX64_PLATFORM_N64_MI_MMIO_ADDRESS_IQUE_SECURE_EXCEPTION =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_MI +
      0x00000014
   );
TUX64_PLATFORM_N64_MI_MMIO_ADDRESS_IQUE_RANDOM =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_MI +
      0x0000002c
   );
TUX64_PLATFORM_N64_MI_MMIO_ADDRESS_IQUE_INTERRUPT =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_MI +
      0x00000038
   );
TUX64_PLATFORM_N64_MI_MMIO_ADDRESS_IQUE_MASK =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_MI +
      0x0000003c
   );

