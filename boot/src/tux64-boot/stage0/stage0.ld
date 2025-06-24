/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage0/stage0.ld - Stage-0 bootloader linker script.   */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/ldscripts/platform/mips/n64/n64.ld

SECTIONS
{
   /* the stage-0 code binary to load in place of the IPL3 */
   .rsp_dmem (TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM + 0x40) : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00

   /* store the status data and .bss items at the start of RSP IMEM */
   .rsp_imem : {
      *(.status);
      *(.bss*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_IMEM =0x00

   /* store CIC data in seperate section to be extracted as binary data */
   .cic : {
      KEEP(*(.cic));
   } =0x00
}

