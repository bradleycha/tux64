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
   Tux64UInt8 framebuffer_index_displaying;
   Tux64UInt8 framebuffer_index_rendering;
};

static struct Tux64BootStage1VideoContext
tux64_boot_stage1_video_context;

static Tux64UInt8
tux64_boot_stage1_video_context_framebuffer_index_displaying(void) {
   return tux64_boot_stage1_video_context.framebuffer_index_displaying;
}

static Tux64UInt8
tux64_boot_stage1_video_context_framebuffer_index_rendering(void) {
   return tux64_boot_stage1_video_context.framebuffer_index_rendering;
}

static Tux64UInt8
tux64_boot_stage1_video_context_framebuffer_index_pending(void) {
   Tux64UInt8 idx_displaying;
   Tux64UInt8 idx_rendering;
   Tux64UInt8 idx_pending;

   idx_displaying = tux64_boot_stage1_video_context_framebuffer_index_displaying();
   idx_rendering  = tux64_boot_stage1_video_context_framebuffer_index_rendering();

   idx_pending = TUX64_LITERAL_UINT8(0u);
   while (idx_pending == idx_displaying || idx_pending == idx_rendering) {
      idx_pending++;
   }

   return idx_pending;
}

void
tux64_boot_stage1_video_initialize(void) {
   /* initialize VI registers, */
   (void)tux64_boot_stage1_video_context_framebuffer_index_pending;
   return;
}

void
tux64_boot_stage1_video_display_output(
   Tux64Boolean state
) {
   /* TODO: implement */
   (void)state;
   return;
}

void
tux64_boot_stage1_video_swap_buffers(void) {
   /* TODO: implement */
   return;
}

struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_get_render_target(void) {
   Tux64UInt8 idx_rendering;

   idx_rendering = tux64_boot_stage1_video_context_framebuffer_index_rendering();

   return &tux64_boot_stage1_video_context.framebuffers[idx_rendering];
}

