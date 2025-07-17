/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/video.c - Framebuffer video implementation.     */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/video.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/n64/mmio.h>
#include "tux64-boot/stage1/interrupt/interrupt.h"

#define TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT\
   (3u) /* triple-buffered */

struct Tux64BootStage1VideoContext {
   struct Tux64BootStage1VideoFramebuffer framebuffers [TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT];
   Tux64UInt8 framebuffer_index_displaying;
   Tux64UInt8 framebuffer_index_pending;
   Tux64UInt8 framebuffer_index_rendering;
};

static struct Tux64BootStage1VideoContext
tux64_boot_stage1_video_context;

static struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_framebuffer_get(
   Tux64UInt8 idx
) {
   struct Tux64BootStage1VideoFramebuffer * retn;

   /* we convert from cached to uncached because the VI works with uncached */
   /* addresses and we run into issues with data not being written back in */
   /* time to be read by the RCP.  particularly, we aren't doing a lot of */
   /* random accesses; only linear filling of the image, so there is little */
   /* value in using the CPU cache.  for large copies/fills, RSP DMA should */
   /* be used, which also leads to issues with the CPU caches. */
   retn = &tux64_boot_stage1_video_context.framebuffers[idx];
   retn = (struct Tux64BootStage1VideoFramebuffer *)tux64_platform_mips_n64_memory_map_direct_cached_to_direct_uncached(retn);

   return retn;
}

static void
tux64_boot_stage1_video_set_vi_framebuffer(
   Tux64UInt8 index
) {
   const void * address;

   address = tux64_boot_stage1_video_framebuffer_get(index);

   tux64_platform_mips_n64_mmio_registers_vi.origin = (Tux64UInt32)address;

   return;
}

static void
tux64_boot_stage1_video_initialize_framebuffer(
   Tux64UInt8 idx
) {
   struct Tux64BootStage1VideoFramebuffer * framebuffer;
   volatile Tux64UInt64 * iter_pixels;
   Tux64UInt32 bytes_remaining;

   framebuffer = tux64_boot_stage1_video_framebuffer_get(idx);

   /* TODO: use RSP DMA to do this.  in comparison, this is painfully slow. */
   iter_pixels = (volatile Tux64UInt64 *)&framebuffer->pixels;
   bytes_remaining = TUX64_LITERAL_UINT32(sizeof(framebuffer->pixels));
   while (bytes_remaining != TUX64_LITERAL_UINT32(0u)) {
      *iter_pixels = TUX64_LITERAL_UINT64(0llu);
      iter_pixels++;
      bytes_remaining -= TUX64_LITERAL_UINT32(sizeof(*iter_pixels));
   }
   
   return;
}

static void
tux64_boot_stage1_video_initialize_context(void) {
   struct Tux64BootStage1VideoContext * ctx;
   Tux64UInt8 i;

   /* done to save on typing */
   ctx = &tux64_boot_stage1_video_context;

   /* initialize non-rendering framebuffers to black which prevents garbage */
   /* from being rendered on screen. */
   i = TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT - 1u);
   while (i != TUX64_LITERAL_UINT8(0u)) {
      tux64_boot_stage1_video_initialize_framebuffer(i);
      i--;
   }

   ctx->framebuffer_index_displaying = TUX64_LITERAL_UINT8(2u);
   ctx->framebuffer_index_pending = TUX64_LITERAL_UINT8(1u);
   ctx->framebuffer_index_rendering = TUX64_LITERAL_UINT8(0u);

   return;
}

static void
tux64_boot_stage1_video_initialize_vi(void) {
   volatile struct Tux64PlatformMipsN64MmioRegistersVi * vi;

   /* done to save on typing */
   vi = &tux64_platform_mips_n64_mmio_registers_vi;

   /* TODO: implement */
   (void)vi;
   return;
}

void
tux64_boot_stage1_video_initialize(void) {
   tux64_boot_stage1_video_initialize_context();
   tux64_boot_stage1_video_initialize_vi();
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
   tux64_boot_stage1_video_set_vi_framebuffer(idx_pending);

   return;
}

struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_get_render_target(void) {
   Tux64UInt8 idx_rendering;

   idx_rendering = tux64_boot_stage1_video_context.framebuffer_index_rendering;

   return tux64_boot_stage1_video_framebuffer_get(idx_rendering);
}

