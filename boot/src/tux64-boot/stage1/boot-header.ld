/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/boot-header.ld - Stage-1 boot header linker     */
/*    script definitions.                                                     */
/*----------------------------------------------------------------------------*/

/* this should be included after stack.ld */

INCLUDE tux64/ldscripts/platform/mips/n64/boot.ld

TUX64_BOOT_STAGE1_BOOT_HEADER_ADDRESS =
   TUX64_BOOT_STAGE1_STACK_ADDRESS + TUX64_BOOT_STAGE1_STACK_BYTES;

SECTIONS
{
   .rdram.boot_header (TUX64_BOOT_STAGE1_BOOT_HEADER_ADDRESS) : {
      tux64_boot_stage1_boot_header = .;
      *(.boot_header);

      /* force the position to be correct if its not already. we align to a */
      /* 16-byte boundary so we can use PI DMA into RDRAM, as well as ensure */
      /* we are on a different cache line than the boot header so we dont */
      /* have to worry about invalidating any parts of cache. */
      . = TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_LENGTH_BYTES_ALIGN16;
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

