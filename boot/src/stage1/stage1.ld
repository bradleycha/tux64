/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage1/stage1.ld - Stage-1 bootloader linker script.              */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/tux64.ld

SECTIONS
{
   /* place the entire stage-1 bootloader at the start of RDRAM */
   .rdram : {
      KEEP(*(.start));
      *(.text* .rodata* .data* .bss*);
   } >TUX64_PLATFORM_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

