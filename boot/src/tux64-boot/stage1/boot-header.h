/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/boot-header.h - Header for globally loaded boot */
/*    header.                                                                 */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_BOOT_HEADER_H
#define _TUX64_BOOT_STAGE1_BOOT_HEADER_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/boot.h>

extern const struct Tux64PlatformMipsN64BootHeader
tux64_boot_stage1_boot_header
__attribute__((section(".boot_header")));

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_BOOT_HEADER_H */

