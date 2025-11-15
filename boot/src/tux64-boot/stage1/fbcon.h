/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/fbcon.h - Interfaces for framebuffer console.   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_FBCON_H
#define _TUX64_BOOT_STAGE1_FBCON_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/video.h"

/*----------------------------------------------------------------------------*/
/* A 6-bit fbcon-encoded character.  Characters are encoded in the following  */
/* format:                                                                    */
/*                                                                            */
/* A-Z =  0-25                                                                */
/* a-z = 26-51                                                                */
/* 0-9 = 52-61                                                                */
/*   % =    62                                                                */
/*   . =    63                                                                */
/*----------------------------------------------------------------------------*/
typedef Tux64UInt8 Tux64BootStage1FbconLabelCharacter;

/*----------------------------------------------------------------------------*/
/* A handle to an allocated text label.  To access members, please use        */
/* tux64_boot_stage1_fbcon_label_character_get() or                           */
/* tux64_boot_stage1_fbcon_label_character_set().                             */
/*----------------------------------------------------------------------------*/
typedef Tux64UInt8 Tux64BootStage1FbconLabel;

/*----------------------------------------------------------------------------*/
/* Pushes a new line of text to the fbcon, placed on the next line after the  */
/* last text label.                                                           */
/*----------------------------------------------------------------------------*/
Tux64BootStage1FbconLabel
tux64_boot_stage1_fbcon_label_push(
   const Tux64BootStage1FbconLabelCharacter * ptr,
   Tux64UInt8 characters
);

/*----------------------------------------------------------------------------*/
/* Get the fbcon-encoded character at index 'idx' within 'label'.             */
/*----------------------------------------------------------------------------*/
Tux64BootStage1FbconLabelCharacter
tux64_boot_stage1_fbcon_label_character_get(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx
);

/*----------------------------------------------------------------------------*/
/* Set the fbcon-encoded character at index 'idx' within 'label' to           */
/* 'character'.                                                               */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_fbcon_label_character_set(
   Tux64BootStage1FbconLabel label,
   Tux64UInt8 idx,
   Tux64BootStage1FbconLabelCharacter character
);

/*----------------------------------------------------------------------------*/
/* Initializes the framebuffer console.  Requires the video subsystem to be   */
/* initialized beforehand.                                                    */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_fbcon_initialize(
   Tux64BootStage1VideoPixel color_foreground,
   Tux64BootStage1VideoPixel color_background
);

/*----------------------------------------------------------------------------*/
/* Renders all text labels in the framebuffer console to the current          */
/* rendering framebuffer.                                                     */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_fbcon_render(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_FBCON_H */

