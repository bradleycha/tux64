/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/text.c - Definitions for fbcon text strings.    */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/text.h"

#include "tux64-boot/stage1/fbcon.h"

/* TODO: create another command-line build tool to generate this data at */
/* compile-time so we don't have to manually transcribe. */

static const Tux64BootStage1FbconLabelCharacter
tux64_boot_stage1_text_data_splash [] = {
   /* Hello world */
   TUX64_LITERAL_UINT8(0x07u),
   TUX64_LITERAL_UINT8(0x1eu),
   TUX64_LITERAL_UINT8(0x25u),
   TUX64_LITERAL_UINT8(0x25u),
   TUX64_LITERAL_UINT8(0x27u),
   TUX64_LITERAL_UINT8(0x40u),
   TUX64_LITERAL_UINT8(0x30u),
   TUX64_LITERAL_UINT8(0x27u),
   TUX64_LITERAL_UINT8(0x2au),
   TUX64_LITERAL_UINT8(0x25u),
   TUX64_LITERAL_UINT8(0x1du)
};

#define TUX64_BOOT_STAGE1_TEXT_DATA_SPLASH_CHARACTERS \
   TUX64_ARRAY_ELEMENTS(tux64_boot_stage1_text_data_splash)

/* all text strings stored next to each other in memory to hopefully improve */
/* caching via locality */
static const struct Tux64BootStage1FbconText
tux64_boot_stage1_text [] = {
   {
      .ptr        = tux64_boot_stage1_text_data_splash,
      .length     = TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_TEXT_DATA_SPLASH_CHARACTERS),
      .capacity   = TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_TEXT_DATA_SPLASH_CHARACTERS)
   }
};

const struct Tux64BootStage1FbconText *
tux64_boot_stage1_text_splash = &tux64_boot_stage1_text[0u];

