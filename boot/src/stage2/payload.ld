/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage2/payload.ld - Stage-2 bootloader payload linker script.     */
/*----------------------------------------------------------------------------*/

/* WARNING: the payload's size must be manually maintained. */
TUX64_BOOT_STAGE2_PAYLOAD_SIZE = 16;

SECTIONS
{
   .stage2.rsp_dmem.payload : {
      tux64_boot_stage2_payload = .;
      *(.stage2.payload);
      . += TUX64_BOOT_STAGE2_PAYLOAD_SIZE;
   } >TUX64_PLATFORM_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

