/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/boot-header.h - Header for globally loaded boot */
/*    header and related functions.                                           */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_BOOT_HEADER_H
#define _TUX64_BOOT_STAGE1_BOOT_HEADER_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/boot.h>
#include <tux64/platform/mips/n64/pi.h>

/*----------------------------------------------------------------------------*/
/* The global boot header loaded into RDRAM, which had its checksum verified  */
/* in stage-0.                                                                */
/*----------------------------------------------------------------------------*/
extern const struct Tux64PlatformMipsN64BootHeader
tux64_boot_stage1_boot_header
__attribute__((section(".boot_header")));

/*----------------------------------------------------------------------------*/
/* Checks if various boot flags are set.                                      */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_boot_header_flag_no_checksum(void);
Tux64Boolean
tux64_boot_stage1_boot_header_flag_memory_display(void);

struct Tux64BootStage1BootHeaderFile {
   Tux64PlatformMipsN64PiBusAddress address;
   Tux64UInt32 bytes;
   Tux64UInt32 checksum;
};

struct Tux64BootStage1BootHeaderFiles {
   struct Tux64BootStage1BootHeaderFile bootloader_stage2;
   struct Tux64BootStage1BootHeaderFile kernel;
   struct Tux64BootStage1BootHeaderFile initramfs;
   struct Tux64BootStage1BootHeaderFile command_line;
};

/*----------------------------------------------------------------------------*/
/* Gets the address, length, and checksum of various boot files with respect  */
/* to the PI bus.                                                             */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1BootHeaderFiles
tux64_boot_stage1_boot_header_files(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_BOOT_HEADER_H */

