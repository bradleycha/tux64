/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.ld - Stage-2 bootloader linker script.   */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/ldscripts/platform/mips/n64/n64.ld

ENTRY(tux64_boot_stage2_start)

SECTIONS
{
   /* place the stage-2 code and data into RSP IMEM */
   .rsp_imem : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_IMEM =0x00

   /* place the stage-2 uninitialized variables into RSP DMEM */
   .rsp_dmem.bss : {
      *(.bss*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

