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
   (2u) /* double-buffered */

struct Tux64BootStage1VideoContext {
   struct Tux64BootStage1VideoFramebuffer framebuffers [TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT];
   volatile Tux64Boolean vblank_fired;
   Tux64UInt8 framebuffer_index_displaying;
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

static Tux64UInt8
tux64_boot_stage1_video_framebuffer_index_get_displaying(void) {
   Tux64UInt8 retn;

   retn = tux64_boot_stage1_video_context.framebuffer_index_displaying;

   return retn;
}

static Tux64UInt8
tux64_boot_stage1_video_framebuffer_index_get_rendering(void) {
   Tux64UInt8 retn;

   retn = tux64_boot_stage1_video_framebuffer_index_get_displaying();
   retn = retn + TUX64_LITERAL_UINT8(1u);
   retn = retn % TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT);

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

   ctx->vblank_fired = TUX64_BOOLEAN_FALSE;
   ctx->framebuffer_index_displaying = TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT - 1u);

   return;
}

static void
tux64_boot_stage1_video_initialize_vi(
   enum Tux64BootStage1VideoPlatform platform
) {
   volatile struct Tux64PlatformMipsN64MmioRegistersVi * vi;

   /* done to save on typing */
   vi = &tux64_platform_mips_n64_mmio_registers_vi;

   /* TODO: implement */
   (void)vi;
   (void)platform;
   return;
}

void
tux64_boot_stage1_video_initialize(
   enum Tux64BootStage1VideoPlatform platform
) {
   tux64_boot_stage1_video_initialize_context();
   tux64_boot_stage1_video_initialize_vi(platform);
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
   struct Tux64BootStage1VideoContext * ctx;
   Tux64UInt8 idx_rendering;

   ctx = &tux64_boot_stage1_video_context;

   /* use a spinlock to wait for vblank to fire, locking the framerate to a */
   /* maximum of 60 FPS. */
   while (ctx->vblank_fired == TUX64_BOOLEAN_FALSE) {}
   ctx->vblank_fired = TUX64_BOOLEAN_FALSE;

   /* the rest of this code shouldn't take longer than 1/60 of a second to */
   /* complete, so no futher synchronization is required */

   idx_rendering = tux64_boot_stage1_video_framebuffer_index_get_rendering();

   ctx->framebuffer_index_displaying = idx_rendering;
   tux64_boot_stage1_video_set_vi_framebuffer(idx_rendering);

   return;
}

void
tux64_boot_stage1_video_vblank_handler(void) {
   tux64_boot_stage1_video_context.vblank_fired = TUX64_BOOLEAN_TRUE;
   return;
}

struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_get_render_target(void) {
   Tux64UInt8 idx;

   idx = tux64_boot_stage1_video_framebuffer_index_get_rendering();

   return tux64_boot_stage1_video_framebuffer_get(idx);
}

