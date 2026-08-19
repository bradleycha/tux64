/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stack.h - Stage-1 stack definitions.            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_H
#define _TUX64_BOOT_STAGE1_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/n64/boot.h>
#include "tux64-boot/header.h"

/*----------------------------------------------------------------------------*/
/* The size of the stack, in bytes.                                           */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE1_STACK_BYTES\
   (0x00001000) /* 4KiB stack, should be plenty */

/*----------------------------------------------------------------------------*/
/* The location of the start of the stack in memory.                          */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE1_STACK_ADDRESS\
   (TUX64_BOOT_HEADER_ADDRESS + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES_ALIGN16)

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_H */

