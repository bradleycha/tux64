/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
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
/* An 8-bit fbcon-encoded character.  Characters are encoded in the following */
/* format:                                                                    */
/*                                                                            */
/* A-Z =  0-25                                                                */
/* a-z = 26-51                                                                */
/* 0-9 = 52-61                                                                */
/*   % =    62                                                                */
/*   . =    63                                                                */
/*     =    64                                                                */
/*----------------------------------------------------------------------------*/
typedef Tux64UInt8 Tux64BootStage1FbconLabelCharacter;

/*----------------------------------------------------------------------------*/
/* A handle to an allocated text label.  To access members, please use        */
/* tux64_boot_stage1_fbcon_label_character_get() or                           */
/* tux64_boot_stage1_fbcon_label_character_set().                             */
/*----------------------------------------------------------------------------*/
typedef void * Tux64BootStage1FbconLabel;

/*----------------------------------------------------------------------------*/
/* A text string used to initialize a label.                                  */
/*----------------------------------------------------------------------------*/
/* ptr -                                                                      */
/*    The pointer to the 8-bit fbcon-encoded text string of length 'length'.  */
/*                                                                            */
/* length -                                                                   */
/*    The number of characters pointed at by 'ptr'.                           */
/*                                                                            */
/* capacity -                                                                 */
/*    The size of the allocated string buffer.  This can be useful for        */
/*    appending text at runtime to a constant string, which can be used for   */
/*    animating text.                                                         */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1FbconText {
   const Tux64BootStage1FbconLabelCharacter * ptr;
   Tux64UInt8 length;
   Tux64UInt8 capacity;
};

/*----------------------------------------------------------------------------*/
/* Pushes a new line of text to the fbcon, placed on the next line after the  */
/* last text label.                                                           */
/*----------------------------------------------------------------------------*/
Tux64BootStage1FbconLabel
tux64_boot_stage1_fbcon_label_push(
   const struct Tux64BootStage1FbconText * text
);

/*----------------------------------------------------------------------------*/
/* Skips a line without printing any text.                                    */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_fbcon_skip_line(void);

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
/* Get the number of characters which can be stored in the allocated memory   */
/* for the text label.                                                        */
/*----------------------------------------------------------------------------*/
Tux64UInt8
tux64_boot_stage1_fbcon_label_capacity_get(
   Tux64BootStage1FbconLabel label
);

/*----------------------------------------------------------------------------*/
/* Encodes an ASCII character into the internal representation for a          */
/* character.  This character must be one of the following:                   */
/*                                                                            */
/*    A-Z                                                                     */
/*    a-z                                                                     */
/*    0-9                                                                     */
/*    '%'                                                                     */
/*    '.'                                                                     */
/*    ' '                                                                     */
/*----------------------------------------------------------------------------*/
Tux64BootStage1FbconLabelCharacter
tux64_boot_stage1_fbcon_character_encode(
   char character
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

