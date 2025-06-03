/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage2/stage2.ld - Stage-2 bootloader linker script.              */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/ldscripts/platform-n64/platform-n64.ld
INCLUDE stage2/payload.ld

SECTIONS
{
   /* place the stage-2 code and data into RSP IMEM */
   .rsp_imem : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } >TUX64_PLATFORM_N64_MEMORY_MAP_LAYOUT_RSP_IMEM =0x00

   /* place the stage-2 uninitialized variables into RSP DMEM after the */
   /* stage-1 payload */
   .rsp_dmem.bss (ADDR(.stage2.rsp_dmem.payload) + SIZEOF(.stage2.rsp_dmem.payload)) : {
      *(.bss*);
   } >TUX64_PLATFORM_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

