/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.ld - Stage-1 bootloader linker script.   */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/ldscripts/platform/mips/n64/n64.ld

SECTIONS
{
   /* place the stage-1 code and data into RSP IMEM */
   .rsp_imem : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_IMEM =0x00

   /* place the stage-1 uninitialized variables at the start of RSP DMEM */
   .rsp_dmem : {
      *(.status);
      *(.bss*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

