/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage2/stage2.ld - Stage-2 bootloader linker script.              */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/platform-n64.ld

SECTIONS
{
   /* place the stage-2 code and data into RSP DMEM */
   .stage2.rsp_dmem : {
      KEEP(*(.start));
      *(.text .rodata .data);
   } >N64_MEMORY_RSP_DMEM =0x00

   /* place the stage-2 uninitialized variables into RSP IMEM */
   .stage2.rsp_imem : {
      *(.bss);
   } >N64_MEMORY_RSP_IMEM =0x00
}

