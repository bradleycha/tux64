/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage0/stage0.ld - Stage-0 bootloader linker script.   */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/ldscripts/platform/mips/n64/n64.ld

INCLUDE tux64-boot/stage1/stack.ld
INCLUDE tux64-boot/stage1/boot-header.ld

ENTRY(tux64_boot_stage0_start)

TUX64_BOOT_STAGE0_MEMORY_ADDRESS_CIC_DATA
 = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM + 0x1000 - 0x08;

SECTIONS
{
   /* the stage-0 code binary to load in place of the IPL3 */
   .rsp_dmem (TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM + 0x40) : {
      KEEP(*(.start));
      *(.text* .rodata* .data*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00

   /* store the status code at the end of RSP DMEM, overwriting the CIC data */
   .rsp_dmem.status (TUX64_BOOT_STAGE0_MEMORY_ADDRESS_CIC_DATA) : {
      *(.status);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00

   /* store the .bss items at the start of RSP IMEM */
   .rsp_imem : {
      *(.bss*);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_IMEM =0x00

   /* store CIC data in seperate section to be extracted as binary data */
   .cic (TUX64_BOOT_STAGE0_MEMORY_ADDRESS_CIC_DATA) : {
      KEEP(*(.cic));
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

