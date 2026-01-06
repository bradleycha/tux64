/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
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

/*----------------------------------------------------------------------------*/
/* Initializes the framebuffer video interface.  This must be called before   */
/* using any other video functions.  Implicitly enables video output.         */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_initialize(
   enum Tux64BootStage1VideoPlatform platform,
   Tux64BootStage1VideoPixel clear_color
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
/* Waits until vblank is active.  All code which relies on vblank should use  */
/* this to block.                                                             */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_vblank_wait(void);

/*----------------------------------------------------------------------------*/
/* Signals the end of vblank.  All code which relies on vblank should call    */
/* this after completion.                                                     */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_vblank_end(void);

/*----------------------------------------------------------------------------*/
/* Returns whether vblank was triggered.  This can be useful for writing CPU  */
/* intensive code which is preemptive with respect to video output.           */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_video_vblank_triggered(void);

/*----------------------------------------------------------------------------*/
/* The code to run when v-blank is triggered.  This should only be called     */
/* from the interrupt handler.                                                */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_vblank_handler(void);

/*----------------------------------------------------------------------------*/
/* Swaps the current active video buffer to be displayed.  This should only   */
/* be called when vblank is active.                                           */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_video_swap_buffers(void);

/*----------------------------------------------------------------------------*/
/* Gets mutable access to the current render-target framebuffer.  This can be */
/* freely modified until you call tux64_boot_stage1_video_swap_buffers().     */
/*                                                                            */
/* WARNING:  This returns the framebuffer address in the cached segment.  If  */
/* you are planning on rendering using the CPU, either writeback the data     */
/* cache or convert to the uncached segment, otherwise you'll get incomplete  */
/* frames displayed on screen!                                                */
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

