/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/memory.h - Stage-2 memory definitions.          */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE2_MEMORY_H
#define _TUX64_BOOT_STAGE2_MEMORY_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/boot.h>
#include "tux64-boot/header.h"

/*----------------------------------------------------------------------------*/
/* The amount of bytes from the start of RDRAM reserved by stage-2.           */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE2_MEMORY_RDRAM_RESERVED \
   (TUX64_BOOT_HEADER_ADDRESS_OFFSET + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES_ALIGN16)
   /* stack is placed first, then the boot header.  everything else is in RSP */
   /* memory, so all memory after the boot header is fair game. */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_MEMORY_H */

