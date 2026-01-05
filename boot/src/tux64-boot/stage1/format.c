/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/format.c - Implementations for fbcon text label */
/*    formatters.                                                             */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/format.h"

#include "tux64-boot/stage1/fbcon.h"

static Tux64BootStage1FbconLabelCharacter
tux64_boot_stage1_format_digit_base10(
   Tux64UInt8 digit
) {
   return tux64_boot_stage1_fbcon_character_encode('0') + digit;
}

static Tux64BootStage1FbconLabelCharacter
tux64_boot_stage1_format_leading_digit_base10(
   Tux64UInt8 value
) {
   return tux64_boot_stage1_format_digit_base10(value % TUX64_LITERAL_UINT8(10u));
}

static void
tux64_boot_stage1_format_repeat_character(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx_base,
   Tux64BootStage1FbconLabelCharacter character,
   Tux64UInt8 count
) {
   do {
      tux64_boot_stage1_fbcon_label_character_set(
         label,
         idx_base + count - TUX64_LITERAL_UINT8(1u),
         character
      );

      count--;
   } while(count != TUX64_LITERAL_UINT8(0u));

   return;
}

static void
tux64_boot_stage1_format_mib_static(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx_base
) {
   /* leading spaces */
   tux64_boot_stage1_format_repeat_character(
      label,
      idx_base,
      tux64_boot_stage1_fbcon_character_encode(' '),
      TUX64_LITERAL_UINT8(3u)
   );

   /* decimal separator */
   tux64_boot_stage1_fbcon_label_character_set(
      label,
      idx_base + TUX64_LITERAL_UINT8(4u),
      tux64_boot_stage1_fbcon_character_encode('.')
   );
   
   tux64_boot_stage1_fbcon_label_character_set(
      label,
      idx_base + TUX64_LITERAL_UINT8(7u),
      tux64_boot_stage1_fbcon_character_encode('M')
   );
   tux64_boot_stage1_fbcon_label_character_set(
      label,
      idx_base + TUX64_LITERAL_UINT8(8u),
      tux64_boot_stage1_fbcon_character_encode('i')
   );
   tux64_boot_stage1_fbcon_label_character_set(
      label,
      idx_base + TUX64_LITERAL_UINT8(9u),
      tux64_boot_stage1_fbcon_character_encode('B')
   );

   return;
}

static void
tux64_boot_stage1_format_mib_dynamic(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx_base,
   Tux64UInt32 value
) {
   Tux64UInt8 mib_whole;
   Tux64UInt8 mib_frac;
   Tux64UInt8 i;

   /* don't ask me to explain this.  it's just a bunch of bullshit to get */
   /* the whole and fractional portion for the mebibytes. */
   mib_whole   = (Tux64UInt8)((value & TUX64_LITERAL_UINT32(0xfff00000u)) >> TUX64_LITERAL_UINT32(20u));
   mib_frac    = (Tux64UInt8)(((value & TUX64_LITERAL_UINT32(0x000fffffu)) * TUX64_LITERAL_UINT32(100u)) / TUX64_LITERAL_UINT32(0x100000u));

   tux64_boot_stage1_fbcon_label_character_set(
      label,
      idx_base + TUX64_LITERAL_UINT8(3u),
      tux64_boot_stage1_format_digit_base10(mib_whole)
   );

   i = TUX64_LITERAL_UINT8(2u);
   do {
      tux64_boot_stage1_fbcon_label_character_set(
         label,
         idx_base + TUX64_LITERAL_UINT8(4u) + i,
         tux64_boot_stage1_format_leading_digit_base10(mib_frac)
      );

      mib_frac /= TUX64_LITERAL_UINT8(10u);
      i--;
   } while (i != TUX64_LITERAL_UINT8(0u));

   return;
}

void
tux64_boot_stage1_format_mib(
   Tux64BootStage1FbconLabel label,
   Tux64UInt32 value
) {
   Tux64UInt8 capacity;
   Tux64UInt8 idx_base;

   capacity = tux64_boot_stage1_fbcon_label_capacity_get(label);

   idx_base = capacity - TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_MIB);

   tux64_boot_stage1_format_mib_static(label, idx_base);
   tux64_boot_stage1_format_mib_dynamic(label, idx_base, value);

   return;
}

void
tux64_boot_stage1_format_percentage_initialize(
   struct Tux64BootStage1FormatPercentageContext * context,
   Tux64UInt32 maximum_value
) {
   context->maximum  = maximum_value;
   context->progress = TUX64_LITERAL_UINT32(0u);
   return;
}

void
tux64_boot_stage1_format_percentage_accumulate(
   struct Tux64BootStage1FormatPercentageContext * context,
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
tux64_boot_stage1_format_percentage_static(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx_base
) {
   /* set ellipses */
   tux64_boot_stage1_format_repeat_character(
      label,
      idx_base,
      tux64_boot_stage1_fbcon_character_encode('.'),
      TUX64_LITERAL_UINT8(3u)
   );

   /* set percentage */
   tux64_boot_stage1_fbcon_label_character_set(
      label,
      idx_base + TUX64_LITERAL_UINT8(6u),
      tux64_boot_stage1_fbcon_character_encode('%')
   );

   return;
}

static Tux64UInt8
tux64_boot_stage1_format_percentage_to_integer(
   const struct Tux64BootStage1FormatPercentageContext * context
) {
   Tux64UInt32 v;

   v = context->progress;

   if (v == context->maximum) {
      return TUX64_LITERAL_UINT8(100u);
   }

   return ((v * TUX64_LITERAL_UINT32(100u)) / context->maximum);
}

static void
tux64_boot_stage1_format_percentage_integer(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 value,
   Tux64UInt8 idx_base
) {
   Tux64UInt8 i;

   /* reverse iteration is the best way to do this. */
   i = idx_base + TUX64_LITERAL_UINT8(3u);

   /* formats digits.  intentionally skips checking for zero on the first */
   /* iteration so that '0%' gets printed correctly. */
   do {
      tux64_boot_stage1_fbcon_label_character_set(
         label,
         i - TUX64_LITERAL_UINT8(1u),
         tux64_boot_stage1_format_leading_digit_base10(value)
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
tux64_boot_stage1_format_percentage_dynamic(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx_base,
   const struct Tux64BootStage1FormatPercentageContext * context
) {
   Tux64UInt8 value;

   value = tux64_boot_stage1_format_percentage_to_integer(context);

   tux64_boot_stage1_format_percentage_integer(
      label,
      value,
      idx_base + TUX64_LITERAL_UINT8(3u) /* skip past ellipses */
   );
   
   return;
}

void
tux64_boot_stage1_format_percentage(
   const struct Tux64BootStage1FormatPercentageContext * context,
   Tux64BootStage1FbconLabel label
) {
   Tux64UInt8 capacity;
   Tux64UInt8 idx_base;

   capacity = tux64_boot_stage1_fbcon_label_capacity_get(label);

   idx_base = capacity - TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_PERCENTAGE);

   tux64_boot_stage1_format_percentage_static(label, idx_base);
   tux64_boot_stage1_format_percentage_dynamic(label, idx_base, context);

   return;
}

