/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.ld - Stage-2 bootloader linker script.   */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/ldscripts/platform/mips/n64/n64.ld
INCLUDE tux64-boot/stage3/payload.ld

SECTIONS
{
   /* place the entire stage-2 bootloader at the start of RDRAM, making room */
   /* for a 4kb stack */
   .rdram (TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED + 0x1000) : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00

   /* include .bss in a seperate section which we don't dump.  that way, it */
   /* won't be included in the binary and waste space.  the downside is that */
   /* we now need to store the length of the binary and the length of the */
   /* allocated memory for the binary seperately. */
   .rdram.bss (ADDR(.rdram) + SIZEOF(.rdram)) : {
      *(.bss*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

