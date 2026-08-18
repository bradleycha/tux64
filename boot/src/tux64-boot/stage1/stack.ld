/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stack.ld - Stage-1 stack linker script          */
/*    definitions.                                                            */
/*----------------------------------------------------------------------------*/

/* this should be included after boot-header.ld */

/* TODO: run preprocessor on linker script to                                 */
/* #include "tux64-boot/stage1/stack.h" and replace this definition with the  */
/* TUX64_BOOT_STAGE1_STACK_BYTES from the header. */
TUX64_BOOT_STAGE1_STACK_BYTES =
   0x00001000;
TUX64_BOOT_STAGE1_STACK_ADDRESS =
   TUX64_BOOT_STAGE1_BOOT_HEADER_ADDRESS + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_LENGTH_BYTES_ALIGN16;
   /* place at the start of RDRAM */

SECTIONS
{
   .rdram.stack (TUX64_BOOT_STAGE1_STACK_ADDRESS) : {
      . += TUX64_BOOT_STAGE1_STACK_BYTES;
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

