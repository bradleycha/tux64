/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/percentage.c - Implementation for text-based    */
/*    percentage calculator.                                                  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/percentage.h"

#include "tux64-boot/stage1/fbcon.h"

void
tux64_boot_stage1_percentage_initialize(
   struct Tux64BootStage1PercentageContext * context,
   Tux64UInt32 maximum_value
) {
   context->maximum  = maximum_value;
   context->progress = TUX64_LITERAL_UINT32(0u);
   return;
}

void
tux64_boot_stage1_percentage_accumulate(
   struct Tux64BootStage1PercentageContext * context,
   Tux64UInt32 value
) {
   /* clamping done cleverly to avoid UB. */
   if (context->progress >= context->maximum - value) {
      context->progress = context->maximum;
      return;
   }

   context->progress += value;
   return;
}

static void
tux64_boot_stage1_percentage_format_static(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx_base
) {
   Tux64UInt8 i;

   /* set ellipses */
   i = TUX64_LITERAL_UINT8(3u);
   do {
      tux64_boot_stage1_fbcon_label_character_set(
         label,
         idx_base + i - TUX64_LITERAL_UINT8(1u),
         tux64_boot_stage1_fbcon_character_encode('.')
      );

      i--;
   } while (i != TUX64_LITERAL_UINT8(0u));

   /* set percentage */
   tux64_boot_stage1_fbcon_label_character_set(
      label,
      idx_base + TUX64_LITERAL_UINT8(6u),
      tux64_boot_stage1_fbcon_character_encode('%')
   );

   return;
}

static Tux64UInt8
tux64_boot_stage1_percentage_to_integer(
   const struct Tux64BootStage1PercentageContext * context
) {
   Tux64UInt32 v;

   v = context->progress;

   if (v == context->maximum) {
      return TUX64_LITERAL_UINT8(100u);
   }

   return ((v * TUX64_LITERAL_UINT32(100u)) / context->maximum);
}

static void
tux64_boot_stage1_percentage_format_integer(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 value,
   Tux64UInt8 idx_base
) {
   char digit;
   Tux64UInt8 i;

   /* reverse iteration is the best way to do this. */
   i = idx_base + TUX64_LITERAL_UINT8(3u);

   /* formats digits.  intentionally skips checking for zero on the first */
   /* iteration so that '0%' gets printed correctly. */
   do {
      digit = '0' + (value % TUX64_LITERAL_UINT8(10u));

      tux64_boot_stage1_fbcon_label_character_set(
         label,
         i - TUX64_LITERAL_UINT8(1u),
         tux64_boot_stage1_fbcon_character_encode(digit)
      );

      value = value / TUX64_LITERAL_UINT8(10u);
      i--;
   } while (value != TUX64_LITERAL_UINT8(0u));

   /* clears unformatted space */
   while (i != idx_base) {
      tux64_boot_stage1_fbcon_label_character_set(
         label,
         i - TUX64_LITERAL_UINT8(1u),
         tux64_boot_stage1_fbcon_character_encode(' ')
      );

      i--;
   }

   return;
}

static void
tux64_boot_stage1_percentage_format_dynamic(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx_base,
   const struct Tux64BootStage1PercentageContext * context
) {
   Tux64UInt8 value;

   value = tux64_boot_stage1_percentage_to_integer(context);

   tux64_boot_stage1_percentage_format_integer(
      label,
      value,
      idx_base + TUX64_LITERAL_UINT8(3u) /* skip past ellipses */
   );
   
   return;
}

void
tux64_boot_stage1_percentage_format(
   const struct Tux64BootStage1PercentageContext * context,
   Tux64BootStage1FbconLabel label
) {
   Tux64UInt8 capacity;
   Tux64UInt8 idx_base;

   capacity = tux64_boot_stage1_fbcon_label_capacity_get(label);

   idx_base = capacity - TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_PERCENTAGE_FORMAT_BUFFER_CHARACTERS);

   tux64_boot_stage1_percentage_format_static(label, idx_base);
   tux64_boot_stage1_percentage_format_dynamic(label, idx_base, context);

   return;
}

