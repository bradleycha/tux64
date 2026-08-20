/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/tux64-boot.ld.h - Global bootloader linker script      */
/*    configuration.                                                          */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_LD_H
#define _TUX64_BOOT_LD_H
/*----------------------------------------------------------------------------*/

/* HACK fix to prevent preprocessor from buggering up section names. */
/* otherwise, CPP defines mips=" 1 ".  i tried undefining from the */
/* command-line, but that doesn't work. */
#undef mips

#include "tux64-boot/tux64-boot.h"

#include <tux64/platform/mips/n64/n64.ld.h>
#include "tux64-boot/status.ld.h"
#include "tux64-boot/header.ld.h"
#include "tux64-boot/load.ld.h"

SECTIONS {
   /* forcefully remove any sections implicitly created by the toolchain      */
   /* which end up getting mapped into the binary, causing issues.            */
   /DISCARD/ : {
      *(.MIPS.abiflags)
   }
}

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_LD_H */

