/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage0/stage0.ld - Stage-1 bootloader linker script.              */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/platform-n64.ld

SECTIONS
{
   /* place the entire stage-1 bootloader at the start of RDRAM */
   .stage1.rdram : {
      KEEP(*(.start));
      *(.text .rodata .data .bss);
   } >N64_MEMORY_RDRAM_CACHED =0x00
}

