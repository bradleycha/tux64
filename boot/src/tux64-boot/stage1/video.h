/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/video.h - Framebuffer video inferface.          */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_VIDEO_H
#define _TUX64_BOOT_STAGE1_VIDEO_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

#define TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_X\
   (320u)
#define TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_Y\
   (240u)
#define TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_TOTAL\
   (\
      TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_X *\
      TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_Y\
   )
#define TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_ALIGNMENT\
   (8u)

/*----------------------------------------------------------------------------*/
/* A single framebuffer which can be directly accessed by the CPU.  Pixels    */
/* are the in RGBA5553 format.                                                */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1VideoFramebuffer {
   /* use RGBA5553 pixel format to save memory */
   Tux64UInt16 pixels [TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_TOTAL]
   __attribute__((aligned(TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_ALIGNMENT)));
};

/*----------------------------------------------------------------------------*/
/* Initializes the framebuffer video interface.  This must be called before   */
/* using any other video functions.                                           */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_initialize(void);

/*----------------------------------------------------------------------------*/
/* Sets whether video output should be enabled or not.  Default is to         */
/* disable.                                                                   */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_display_output(
   Tux64Boolean state
);

/*----------------------------------------------------------------------------*/
/* Swaps the current active video buffer to be displayed.                     */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_swap_buffers(void);

/*----------------------------------------------------------------------------*/
/* Gets mutable access to the current render-target framebuffer.  This can be */
/* freely modified until you call tux64_boot_stage1_video_swap_buffers().     */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_get_render_target(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_VIDEO_H */

