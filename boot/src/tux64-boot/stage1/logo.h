/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/logo.h - Header for logo rendering.             */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_LOGO_H
#define _TUX64_BOOT_STAGE1_LOGO_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/video.h"

/*----------------------------------------------------------------------------*/
/* Initializes the logo.  Requires the video subsystem to be initialized      */
/* beforehand.                                                                */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_logo_initialize(
   Tux64BootStage1VideoPixel color_transparent
);

/*----------------------------------------------------------------------------*/
/* Renders the logo to the current rendering framebuffer's top-right corner.  */
/* The RSP DMA engine must be available to queue a DMA transfer when called,  */
/* and a DMA transfer may still be ongoing in the background.                 */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_logo_render(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_LOGO_H */

