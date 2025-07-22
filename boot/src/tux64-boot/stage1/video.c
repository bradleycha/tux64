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
#include <tux64/platform/mips/n64/vi.h>
#include <tux64/bitwise.h>
#include "tux64-boot/stage1/interrupt/interrupt.h"

#define TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT\
   (2u) /* double-buffered */

struct Tux64BootStage1VideoContext {
   struct Tux64BootStage1VideoFramebuffer framebuffers [TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT];
   Tux64UInt8 framebuffer_index_displaying;
   volatile Tux64Boolean swap_requested;
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

   ctx->framebuffer_index_displaying = TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT - 1u);
   ctx->swap_requested = TUX64_BOOLEAN_FALSE;

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
   Tux64UInt32 ctrl;
   Tux64UInt32 type;

   ctrl = tux64_platform_mips_n64_mmio_registers_vi.ctrl;
   ctrl = tux64_bitwise_flags_clear_uint32(
      ctrl,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_VI_TYPE_MASK)
   );

   switch (state) {
      case TUX64_BOOLEAN_FALSE:
         type = TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_VI_TYPE_DISABLED);
         break;
      case TUX64_BOOLEAN_TRUE:
         type = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXEL_FORMAT);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   ctrl = tux64_bitwise_flags_set_uint32(ctrl, type);
   tux64_platform_mips_n64_mmio_registers_vi.ctrl = ctrl;

   (void)state;
   return;
}

void
tux64_boot_stage1_video_swap_buffers(void) {
   struct Tux64BootStage1VideoContext * ctx;

   ctx = &tux64_boot_stage1_video_context;

   /* spinlock to wait for vblank to handle the swap, done to prevent screen */
   /* tearing under lag conditions.  the spinlock also locks the framerate to */
   /* 60FPS. */
   ctx->swap_requested = TUX64_BOOLEAN_TRUE;
   while (ctx->swap_requested == TUX64_BOOLEAN_TRUE) {};

   return;
}

void
tux64_boot_stage1_video_vblank_handler(void) {
   struct Tux64BootStage1VideoContext * ctx;
   Tux64UInt8 idx_rendering;

   ctx = &tux64_boot_stage1_video_context;

   /* new frame isn't available? don't swap buffers. */
   if (ctx->swap_requested == TUX64_BOOLEAN_FALSE) {
      return;
   }

   /* swap buffers since we have a new frame ready */
   idx_rendering = tux64_boot_stage1_video_framebuffer_index_get_rendering();
   ctx->framebuffer_index_displaying = idx_rendering;
   tux64_boot_stage1_video_set_vi_framebuffer(idx_rendering);

   /* signals to tux64_boot_stage1_video_swap_buffers() that the swap is */
   /* complete and to exit its spinlock. */
   ctx->swap_requested = TUX64_BOOLEAN_FALSE;

   return;
}

struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_get_render_target(void) {
   Tux64UInt8 idx;

   idx = tux64_boot_stage1_video_framebuffer_index_get_rendering();

   return tux64_boot_stage1_video_framebuffer_get(idx);
}

