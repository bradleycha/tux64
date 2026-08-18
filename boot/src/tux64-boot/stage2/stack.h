/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stack.h - Stage-2 stack definitions.            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE2_STACK_H
#define _TUX64_BOOT_STAGE2_STACK_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/memory-map.h>
#include "tux64-boot/header.h"

/*----------------------------------------------------------------------------*/
/* The size of the stack, in bytes.                                           */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE2_STACK_BYTES\
   TUX64_BOOT_HEADER_ADDRESS_OFFSET
   /* since we don't need exception processing in stage-2, we can make use of */
   /* the reserved chunk of memory before the boot header for the stack, thus */
   /* freeing more memory for the boot files. */

/*----------------------------------------------------------------------------*/
/* The location of the start of the stack in memory.                          */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE2_STACK_ADDRESS\
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_STACK_H */

