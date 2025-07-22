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
#include <tux64/platform/mips/n64/vi.h>

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
#define TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXEL_FORMAT\
   TUX64_PLATFORM_MIPS_N64_VI_TYPE_RGBA5553

/*----------------------------------------------------------------------------*/
/* A single framebuffer which can be directly accessed by the CPU.  Pixels    */
/* are the in RGBA5553 format.                                                */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1VideoFramebuffer {
   /* use RGBA5553 pixel format to save memory.  marked volatile since it can */
   /* be observed externally and also modified by RSP DMA. */
   volatile Tux64UInt16 pixels [TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_TOTAL]
   __attribute__((aligned(TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_ALIGNMENT)));
};

enum Tux64BootStage1VideoPlatform {
   /* manually define for compatability with IPL2 video standard enum */
   TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_PAL  = 0u,
   TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_NTSC = 1u,
   TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_MPAL = 2u,
   TUX64_BOOT_STAGE1_VIDEO_PLATFORM_IQUE     = 3u
};

/*----------------------------------------------------------------------------*/
/* Initializes the framebuffer video interface.  This must be called before   */
/* using any other video functions.  Implicitly enables video output.         */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_initialize(
   enum Tux64BootStage1VideoPlatform platform
);

/*----------------------------------------------------------------------------*/
/* Sets whether video output should be enabled or not.  Default is to         */
/* enable.                                                                    */
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
/* The code to run when v-blank is triggered.  This should only be called     */
/* from the interrupt handler.                                                */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_vblank_handler(void);

/*----------------------------------------------------------------------------*/
/* Gets mutable access to the current render-target framebuffer.  This can be */
/* freely modified until you call tux64_boot_stage1_video_swap_buffers().     */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_get_render_target(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_VIDEO_H */

