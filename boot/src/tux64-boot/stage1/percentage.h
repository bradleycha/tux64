/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/percentage.h - Header for text-based animated   */
/*    percentage calculator for measuring progress.                           */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_PERCENTAGE_H
#define _TUX64_BOOT_STAGE1_PERCENTAGE_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/fbcon.h"
#include <tux64/string.h>

#define TUX64_BOOT_STAGE1_PERCENTAGE_FORMAT_BUFFER_CHARACTERS \
   TUX64_STRING_CHARACTERS("...XXX%")

struct Tux64BootStage1PercentageContext {
   /* represents the fractional portion of a decimal number, where */
   /* TUX64_UINT32_MAX represents 100% and 0u represents 0%. */
   Tux64UInt32 progress;
};

/*----------------------------------------------------------------------------*/
/* Initializes the percentage to 0%.                                          */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_percentage_initialize(
   struct Tux64BootStage1PercentageContext * context
);

/*----------------------------------------------------------------------------*/
/* Accumulates progress into the percentage value, clamping at 100%.  This    */
/* value uses the same encoding scheme as the internal progress field.        */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_percentage_accumulate(
   struct Tux64BootStage1PercentageContext * context,
   Tux64UInt32 value
);

/*----------------------------------------------------------------------------*/
/* Formats the current percentage at the end of a text label.  Requires       */
/* TUX64_BOOT_STAGE1_PERCENTAGE_FORMAT_BUFFER_CHARACTERS number of free       */
/* characters at the end to format correctly.                                 */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_percentage_format(
   const struct Tux64BootStage1PercentageContext * context,
   Tux64BootStage1FbconLabel label
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_PERCENTAGE_H */

