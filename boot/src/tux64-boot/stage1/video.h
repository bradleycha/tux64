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
/* A single pixel in the expected pixel format.                               */
/*----------------------------------------------------------------------------*/
typedef Tux64UInt16 Tux64BootStage1VideoPixel; /* RGBA5551 */

/*----------------------------------------------------------------------------*/
/* A single framebuffer which can be directly accessed by the CPU.  Pixels    */
/* are the in RGBA5553 format.                                                */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1VideoFramebuffer {
   /* marked volatile since it can be observed externally and also modified */
   /* by RSP DMA. */
   volatile Tux64BootStage1VideoPixel pixels [TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_TOTAL]
   __attribute__((aligned(TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_ALIGNMENT)));
};

enum Tux64BootStage1VideoPlatform {
   /* manually define for compatability with IPL2 video standard enum */
   TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_PAL  = 0u,
   TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_NTSC = 1u,
   TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_MPAL = 2u,
   TUX64_BOOT_STAGE1_VIDEO_PLATFORM_IQUE     = 3u
};

enum Tux64BootStage1VideoClearColor {
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_BLACK    = 0x0001u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_WHITE    = 0xffffu,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_GRAY     = 0x8421u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_MAGENTA  = 0xd829u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_RED      = 0xf801u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_ORANGE   = 0xfbc1u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_YELLOW   = 0xffc1u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_GREEN    = 0x07c1u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_CYAN     = 0x0529u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_BLUE     = 0x003fu,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_INDIGO   = 0x2823u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_VIOLET   = 0x8835u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_PURPLE   = 0x5029u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_RUBY     = 0x4801u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_EMERALD  = 0x0241u,
   TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_SAPPHIRE = 0x0013u
};

/*----------------------------------------------------------------------------*/
/* Initializes the framebuffer video interface.  This must be called before   */
/* using any other video functions.  Implicitly enables video output.         */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_initialize(
   enum Tux64BootStage1VideoPlatform platform,
   enum Tux64BootStage1VideoClearColor clear_color
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
tux64_boot_stage1_video_render_target_get(void);

/*----------------------------------------------------------------------------*/
/* Clear the current render-target framebuffer to the clear color specified   */
/* on initialization.                                                         */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_render_target_clear(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_VIDEO_H */

