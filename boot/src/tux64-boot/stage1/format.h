/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/format.h - Header for fbcon text label          */
/*    formatters.                                                             */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_FORMAT_H
#define _TUX64_BOOT_STAGE1_FORMAT_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/fbcon.h"
#include <tux64/string.h>

#define TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_MIB \
   TUX64_STRING_CHARACTERS("   X.XXMiB")

/*----------------------------------------------------------------------------*/
/* Formats the number of mebibytes a value is, with two decimals.             */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_format_mib(
   Tux64BootStage1FbconLabel label,
   Tux64UInt32 value
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_FORMAT_H */

