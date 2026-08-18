/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/header.h - Header for globally loaded boot      */
/*    header and related functions.                                           */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_HEADER_H
#define _TUX64_BOOT_HEADER_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/boot.h>
#include <tux64/platform/mips/n64/memory-map.h>
#include "tux64-boot/stage1/stack.h"

#define TUX64_BOOT_HEADER_ADDRESS_OFFSET \
   (0x200u) /* place right after exception vectors, with a little padding for safety. */

#define TUX64_BOOT_HEADER_ADDRESS \
   (TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED + TUX64_BOOT_HEADER_ADDRESS_OFFSET)

#if !TUX64_PREPROCESSOR_ONLY
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Checks if various boot flags are set.                                      */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_header_flag_no_checksum(void);
Tux64Boolean
tux64_boot_header_flag_memory_display(void);
Tux64Boolean
tux64_boot_header_flag_no_delay(void);

/*----------------------------------------------------------------------------*/
/* Gets the information of various boot files.                                */
/*----------------------------------------------------------------------------*/
const struct Tux64PlatformMipsN64BootHeaderFile *
tux64_boot_header_file_bootloader_stage2(void);
const struct Tux64PlatformMipsN64BootHeaderFileKernel *
tux64_boot_header_file_kernel(void);
const struct Tux64PlatformMipsN64BootHeaderFile *
tux64_boot_header_file_initramfs(void);
const struct Tux64PlatformMipsN64BootHeaderFile *
tux64_boot_header_file_command_line(void);

/*----------------------------------------------------------------------------*/
#endif /* !TUX64_PREPROCESSOR_ONLY */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_HEADER_H */

