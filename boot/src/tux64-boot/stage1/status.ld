/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/status.ld - Stage-1 status code linker script   */
/*    definitions.                                                            */
/*----------------------------------------------------------------------------*/

TUX64_BOOT_STAGE1_MEMORY_ADDRESS_STATUS
 = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM + 0x1000 - 0x08;

SECTIONS
{
   /* store at the end of RSP DMEM, which is in the same place as the stage-0 */
   /* bootloader's status code */
   .rsp_dmem.status (TUX64_BOOT_STAGE1_MEMORY_ADDRESS_STATUS) : {
      *(.status);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

