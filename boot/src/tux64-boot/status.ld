/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/status.ld - Boot status code linker script             */
/*    definitions.                                                            */
/*----------------------------------------------------------------------------*/

TUX64_BOOT_MEMORY_ADDRESS_STATUS
 = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM + 0x1000 - 0x08;

SECTIONS
{
   .rsp_dmem.status (TUX64_BOOT_MEMORY_ADDRESS_STATUS) : {
      *(.tux64_boot.status);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

