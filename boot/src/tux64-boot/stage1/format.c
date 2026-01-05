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

static void
tux64_boot_stage1_format_mib_static(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx_base
) {
   Tux64UInt8 i;

   /* leading spaces */
   i = idx_base + TUX64_LITERAL_UINT8(3u);
   do {
      tux64_boot_stage1_fbcon_label_character_set(
         label,
         i - TUX64_LITERAL_UINT8(1u),
         tux64_boot_stage1_fbcon_character_encode(' ')
      );

      i--;
   } while (i != idx_base);

   /* decimal separator */
   tux64_boot_stage1_fbcon_label_character_set(
      label,
      idx_base + TUX64_LITERAL_UINT8(4u),
      tux64_boot_stage1_fbcon_character_encode('.')
   );

   /* 'MiB' label */
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
   mib_whole   = (Tux64UInt8)((value & TUX64_LITERAL_UINT32(0x00f00000u)) >> TUX64_LITERAL_UINT32(20u));
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
         tux64_boot_stage1_format_digit_base10(mib_frac % TUX64_LITERAL_UINT8(10u))
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

