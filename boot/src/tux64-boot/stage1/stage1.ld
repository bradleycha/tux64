/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.ld - Stage-1 bootloader linker script.   */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/ldscripts/platform/mips/n64/n64.ld

INCLUDE tux64-boot/status.ld
INCLUDE tux64-boot/stage1/stack.ld
INCLUDE tux64-boot/stage1/boot-header.ld

ENTRY(tux64_boot_stage1_start)
         
SECTIONS
{
   /* place the main stage-1 binary after the boot header */
   .rdram (ADDR(.rdram.boot_header) + SIZEOF(.rdram.boot_header)) : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);

      /* tells the linker what $gp should be set to, as well as our code what */
      /* to initialize $gp to.  we add 0x8000 so we can make full use of the */
      /* address offset range [-0x8000, 0x7fff].  we align to an 8-byte */
      /* boundary so all memory access widths are supported. */
      _gp = ALIGN(8) + 0x8000;

      *(.sdata*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00

   /* include .bss in a seperate section which we don't dump.  that way, it */
   /* won't be included in the binary and waste space.  the downside is that */
   /* we now need to store the length of the binary and the length of the */
   /* allocated memory for the binary seperately. */
   .rdram.bss (ADDR(.rdram) + SIZEOF(.rdram)) : {
      /* small BSS must be first so as much of it as possible can be reached */
      /* after small data. */
      *(.sbss*);

      *(.bss*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

