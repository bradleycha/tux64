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

PHDRS
{
   /* all non-image sections should be placed in one of these. */
   noload_rdram_cached  PT_NULL AT(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED)   FLAGS(7);
   noload_rsp_dmem      PT_NULL AT(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM)       FLAGS(7);
   noload_rsp_imem      PT_NULL AT(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_IMEM)       FLAGS(7);
}

SECTIONS
{
   /* forcefully remove any sections implicitly created by the toolchain      */
   /* which end up getting mapped into the binary, causing issues.            */
   /DISCARD/ : {
      *(.MIPS.abiflags)
   }
}

/* these must be included after the above so our definitions are available. */
#include "tux64-boot/status.ld.h"
#include "tux64-boot/header.ld.h"
#include "tux64-boot/load.ld.h"
#include "tux64-boot/exec.ld.h"

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_LD_H */

