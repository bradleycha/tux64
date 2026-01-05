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

#define TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_PERCENTAGE \
   TUX64_STRING_CHARACTERS("...XXX%")

struct Tux64BootStage1FormatPercentageContext {
   Tux64UInt32 maximum;
   Tux64UInt32 progress;
};

/*----------------------------------------------------------------------------*/
/* Initializes the percentage, setting the target value which represents 100% */
/* completion.  Starts as 0% completion.  'maximum value' must be greater     */
/* than zero, and less than or equal to floor(TUX64_UINT32_MAX / 100).        */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_format_percentage_initialize(
   struct Tux64BootStage1FormatPercentageContext * context,
   Tux64UInt32 maximum_value
);

/*----------------------------------------------------------------------------*/
/* Accumulates progress into the percentage value, clamping at 100%.          */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_format_percentage_accumulate(
   struct Tux64BootStage1FormatPercentageContext * context,
   Tux64UInt32 value
);

/*----------------------------------------------------------------------------*/
/* Formats the current percentage at the end of a text label.  Requires       */
/* TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_PERCENTAGE number of free       */
/* characters at the end to format correctly.                                 */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_format_percentage(
   const struct Tux64BootStage1FormatPercentageContext * context,
   Tux64BootStage1FbconLabel label
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_FORMAT_H */

