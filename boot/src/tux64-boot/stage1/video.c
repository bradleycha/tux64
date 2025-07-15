/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/video.c - Framebuffer video implementation.     */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/video.h"

#include <tux64/platform/mips/n64/mmio.h>
#include "tux64-boot/stage1/interrupt.h"

#define TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT\
   (3u) /* triple-buffered */

struct Tux64BootStage1VideoContext {
   struct Tux64BootStage1VideoFramebuffer framebuffers [TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT];
   Tux64UInt8 framebuffer_index_displaying;
   Tux64UInt8 framebuffer_index_rendering;
   Tux64UInt8 framebuffer_index_pending;
};

static struct Tux64BootStage1VideoContext
tux64_boot_stage1_video_context;

void
tux64_boot_stage1_video_initialize(void) {
   /* TODO: implement */
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
   Tux64UInt32 idx_rendering;
   Tux64UInt32 idx_pending;

   /* disable VI interrupts to prevent interrupting buffer swapping, which */
   /* can cause problems. */
   tux64_boot_stage1_interrupt_vi_disable();

   /* swap the rendering and pending buffers */
   idx_rendering = tux64_boot_stage1_video_context.framebuffer_index_rendering;
   idx_pending = tux64_boot_stage1_video_context.framebuffer_index_pending;

   tux64_boot_stage1_video_context.framebuffer_index_rendering = idx_pending;
   tux64_boot_stage1_video_context.framebuffer_index_pending = idx_rendering;

   /* re-enable VI interrupts after we disabled them at the start*/
   tux64_boot_stage1_interrupt_vi_enable();
   return;
}

void
tux64_boot_stage1_video_vblank_handler(void) {
   Tux64UInt32 idx_displaying;
   Tux64UInt32 idx_pending;

   /* we will never run this while swapping buffers since that function */
   /* disables VI interrupts, so we're safe to swap the displaying and */
   /* pending buffers. */
   idx_displaying = tux64_boot_stage1_video_context.framebuffer_index_displaying;
   idx_pending = tux64_boot_stage1_video_context.framebuffer_index_pending;

   tux64_boot_stage1_video_context.framebuffer_index_displaying = idx_pending;
   tux64_boot_stage1_video_context.framebuffer_index_pending = idx_displaying;

   /* now tell the VI to use the new framebuffer */
   tux64_platform_mips_n64_mmio_registers_vi.origin = ((Tux64UInt32)(
      &tux64_boot_stage1_video_context.framebuffers[idx_pending]
   ));

   return;
}

struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_get_render_target(void) {
   Tux64UInt8 idx_rendering;

   idx_rendering = tux64_boot_stage1_video_context.framebuffer_index_rendering;

   return &tux64_boot_stage1_video_context.framebuffers[idx_rendering];
}

