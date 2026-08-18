/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/header.ld - Boot header linker script definitions.     */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/ldscripts/platform/mips/n64/boot.ld

/* TODO: run preprocessor on linker script to                                 */
/* #include "tux64-boot/header.h" and replace this definition with the        */
/* TUX64_BOOT_HEADER_ADDRESS from the header.                                 */
TUX64_BOOT_HEADER_ADDRESS =
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED + 0x200;

SECTIONS
{
   .rdram.boot_header (TUX64_BOOT_HEADER_ADDRESS) : {
      tux64_boot_header = .;

      /* force the position to be correct if its not already. we align to a */
      /* 16-byte boundary so we can use PI DMA into RDRAM, as well as ensure */
      /* we are on a different cache line than the boot header so we dont */
      /* have to worry about invalidating any parts of cache. */
      . = TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_LENGTH_BYTES_ALIGN16;
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

