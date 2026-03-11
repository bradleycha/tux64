/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/tux64-boot.ld - Global bootloader linker script        */
/*    configuration.                                                          */
/*----------------------------------------------------------------------------*/

SECTIONS {
   /* forcefully remove any sections implicitly created by the toolchain      */
   /* which end up getting mapped into the binary, causing issues.            */
   /DISCARD/ : {
      *(.MIPS.abiflags)
   }
}

