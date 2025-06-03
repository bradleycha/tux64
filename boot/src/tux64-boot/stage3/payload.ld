/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage3/payload.ld - Stage-3 bootloader payload linker  */
/*    script.                                                                 */
/*----------------------------------------------------------------------------*/

/* WARNING: the payload's size must be manually maintained. */
TUX64_BOOT_STAGE3_PAYLOAD_SIZE = 16;

SECTIONS
{
   .stage3.rsp_dmem.payload : {
      tux64_boot_stage3_payload = .;
      *(.stage3.payload);
      . += TUX64_BOOT_STAGE3_PAYLOAD_SIZE;
   } >TUX64_PLATFORM_N64_MEMORY_MAP_LAYOUT_RSP_DMEM =0x00
}

