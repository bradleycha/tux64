/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stack.ld - Stage-1 stack linker script          */
/*    definitions.                                                            */
/*----------------------------------------------------------------------------*/

TUX64_BOOT_STAGE1_STACK_BYTES =
   0x00001000;
   /* 4KiB stack, should be plenty */
TUX64_BOOT_STAGE1_STACK_ADDRESS =
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED;
   /* place at the start of RDRAM */

SECTIONS
{
   .rdram.stack (TUX64_BOOT_STAGE1_STACK_ADDRESS) : {
      . += TUX64_BOOT_STAGE1_STACK_BYTES;
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

