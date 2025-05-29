/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage0/stage0.ld - Stage-0 bootloader linker script.              */
/*----------------------------------------------------------------------------*/

/* TODO: use tools dir linker script instead of local path */
INCLUDE n64.ld

SECTIONS
{
   /* the stage-0 code binary to load in place of the IPL3 */
   .stage0.rsp_dmem (N64_ADDRESS_VIRTUAL_RSP_DMEM + 0x40) : {
      KEEP(*(.start));
      *(.text .rodata .data);
   } >N64_MEMORY_RSP_DMEM =0x00

   /* store .bss items at the start of RSP IMEM */
   .stage0.rsp_imem : {
      *(.bss);
   } >N64_MEMORY_RSP_IMEM =0x00

   /* store CIC data in seperate section to be extracted as binary data */
   .stage0.cic : {
      KEEP(*(.cic));
   } =0x00
}

