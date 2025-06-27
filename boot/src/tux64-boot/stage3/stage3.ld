/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage3/stage3.ld - Stage-3 bootloader linker script.   */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/ldscripts/platform/mips/n64/n64.ld
INCLUDE tux64-boot/stage3/payload.ld

SECTIONS
{
   /* place the stage-3 code and data into RSP IMEM */
   .rsp_imem : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_IMEM =0x00

   /* place the stage-3 uninitialized variables into RSP DMEM after the */
   /* stage-2 payload */
   .rsp_dmem.bss (ADDR(.stage3.rsp_dmem.payload) + SIZEOF(.stage3.rsp_dmem.payload)) : {
      *(.bss*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

