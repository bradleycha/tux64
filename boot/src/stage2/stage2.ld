/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage2/stage2.ld - Stage-2 bootloader linker script.              */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/tux64.ld

SECTIONS
{
   /* place the stage-2 code and data into RSP DMEM */
   .rsp_dmem : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } >TUX64_PLATFORM_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00

   /* place the stage-2 uninitialized variables into RSP IMEM */
   .rsp_imem : {
      *(.bss*);
   } >TUX64_PLATFORM_N64_MEMORY_MAP_LAYOUT_RSP_IMEM =0x00
}

