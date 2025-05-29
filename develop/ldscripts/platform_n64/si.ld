/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* develop/ldscripts/platform_n64/si.ld - Serial Interface (SI) linker        */
/*    script.                                                                 */
/*----------------------------------------------------------------------------*/

/* addresses of SI MMIO registers */
TUX64_PLATFORM_N64_SI_MMIO_ADDRESS_DRAM_ADDR =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +
      0x00000000
   );
TUX64_PLATFORM_N64_SI_MMIO_ADDRESS_PIF_AD_RD64B =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +
      0x00000004
   );
TUX64_PLATFORM_N64_SI_MMIO_ADDRESS_PIF_AD_WR4B =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +
      0x00000008
   );
TUX64_PLATFORM_N64_SI_MMIO_ADDRESS_PIF_AD_WR64B =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +
      0x00000010
   );
TUX64_PLATFORM_N64_SI_MMIO_ADDRESS_PIF_AD_RD4B =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +
      0x00000014
   );
TUX64_PLATFORM_N64_SI_MMIO_ADDRESS_STATUS =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_SI +
      0x00000018
   );

