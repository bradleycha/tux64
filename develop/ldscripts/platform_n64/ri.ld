/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* develop/ldscripts/platform_n64/ri.ld - RDRAM Interface (RI) linker script. */
/*----------------------------------------------------------------------------*/

/* addresses of RI MMIO registers */
TUX64_PLATFORM_N64_RI_MMIO_ADDRESS_MODE =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_RI +
      0x00000000
   );
TUX64_PLATFORM_N64_RI_MMIO_ADDRESS_CONFIG =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_RI +
      0x00000004
   );
TUX64_PLATFORM_N64_RI_MMIO_ADDRESS_CURRENT_LOAD =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_RI +
      0x00000008
   );
TUX64_PLATFORM_N64_RI_MMIO_ADDRESS_SELECT =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_RI +
      0x0000000c
   );
TUX64_PLATFORM_N64_RI_MMIO_ADDRESS_REFRESH =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_RI +
      0x00000010
   );
TUX64_PLATFORM_N64_RI_MMIO_ADDRESS_LATENCY =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_RI +
      0x00000014
   );
TUX64_PLATFORM_N64_RI_MMIO_ADDRESS_ERROR =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_RI +
      0x00000018
   );
TUX64_PLATFORM_N64_RI_MMIO_ADDRESS_BANK_STATUS =
   (
      TUX64_PLATFORM_N64_MEMORY_MAP_ADDRESS_RI +
      0x0000001c
   );

