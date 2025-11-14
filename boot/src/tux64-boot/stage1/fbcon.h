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
/* Initializes the framebuffer console.  Requires the video subsystem to be   */
/* initialized beforehand.                                                    */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_fbcon_initialize(
   Tux64BootStage1VideoPixel color_foreground,
   Tux64BootStage1VideoPixel color_background
);

/*----------------------------------------------------------------------------*/
/* Renders all text in the framebuffer console to the current rendering       */
/* framebuffer.                                                               */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_fbcon_render(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_FBCON_H */

