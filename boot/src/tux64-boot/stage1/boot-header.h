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

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_BOOT_HEADER_H */

