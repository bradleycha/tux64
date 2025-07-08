/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/video.c - Framebuffer video implementation.     */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/video.h"

#define TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT\
   (3u) /* triple-buffered */

struct Tux64BootStage1VideoContext {
   struct Tux64BootStage1VideoFramebuffer framebuffers [TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT];

   /* bits 0-1: index of the framebuffer currently being presented on-screen */
   /* bits 2-3: index of the framebuffer currently being rendered to */
   Tux64UInt8 framebuffer_active_indices;
};

static struct Tux64BootStage1VideoContext
tux64_boot_stage1_video_context;

void
tux64_boot_stage1_video_initialize(void) {
   /* TODO: implement */
   (void)tux64_boot_stage1_video_context;
   return;
}

void
tux64_boot_stage1_video_swap_buffers(void) {
   /* TODO: implement */
   return;
}

struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_get_render_target(void) {
   /* TODO: implement */
   return TUX64_NULLPTR;
}

