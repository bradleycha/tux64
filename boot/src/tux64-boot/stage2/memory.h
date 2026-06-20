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
#include "tux64-boot/stage2/stack.h"

/*----------------------------------------------------------------------------*/
/* The amount of bytes from the start of RDRAM reserved by stage-2.           */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE2_MEMORY_RDRAM_RESERVED \
   (TUX64_BOOT_STAGE2_STACK_BYTES)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_MEMORY_H */

