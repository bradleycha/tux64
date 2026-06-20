/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stack.ld - Stage-2 stack linker script          */
/*    definitions.                                                            */
/*----------------------------------------------------------------------------*/

/* TODO: same comments about duplicated code from stage1/stack.ld */
TUX64_BOOT_STAGE2_STACK_BYTES =
   0x100;
TUX64_BOOT_STAGE2_STACK_ADDRESS =
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED;

SECTIONS
{
   .rdram.stack (TUX64_BOOT_STAGE2_STACK_ADDRESS) : {
      . += TUX64_BOOT_STAGE2_STACK_BYTES;
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

