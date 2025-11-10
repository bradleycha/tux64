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
#include <tux64/endian.h>
#include "tux64-boot/stage1/interrupt/interrupt.h"

#define TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT\
   (2u) /* double-buffered */

#define TUX64_BOOT_STAGE1_VIDEO_PIXEL_ADVANCE_DEFAULT\
   (0x3u)
#define TUX64_BOOT_STAGE1_VIDEO_PIXEL_ADVANCE_IQUE\
   (0x1u)

#define TUX64_BOOT_STAGE1_VIDEO_BURST_PAL\
   (0x0404233au)
#define TUX64_BOOT_STAGE1_VIDEO_BURST_NTSC\
   (0x03e52239u)

#define TUX64_BOOT_STAGE1_VIDEO_SCANLINES_PAL\
   (626u)
#define TUX64_BOOT_STAGE1_VIDEO_SCANLINES_NTSC\
   (526u)

#define TUX64_BOOT_STAGE1_VIDEO_H_TOTAL_PAL\
   (3118u)
#define TUX64_BOOT_STAGE1_VIDEO_H_TOTAL_NTSC\
   (3094u)
#define TUX64_BOOT_STAGE1_VIDEO_H_TOTAL_MPAL\
   (3090u)

#define TUX64_BOOT_STAGE1_VIDEO_H_START_PAL\
   (128u)
#define TUX64_BOOT_STAGE1_VIDEO_H_END_PAL\
   (768u)
#define TUX64_BOOT_STAGE1_VIDEO_H_START_NTSC\
   (108u)
#define TUX64_BOOT_STAGE1_VIDEO_H_END_NTSC\
   (748u)

#define TUX64_BOOT_STAGE1_VIDEO_V_START_PAL\
   (0x05fu)
#define TUX64_BOOT_STAGE1_VIDEO_V_END_PAL\
   (0x239u)
#define TUX64_BOOT_STAGE1_VIDEO_V_START_NTSC\
   (0x025u)
#define TUX64_BOOT_STAGE1_VIDEO_V_END_NTSC\
   (0x1ffu)

#define TUX64_BOOT_STAGE1_VIDEO_V_BURST_START_PAL\
   (0x009u)
#define TUX64_BOOT_STAGE1_VIDEO_V_BURST_END_PAL\
   (0x26bu)
#define TUX64_BOOT_STAGE1_VIDEO_V_BURST_START_NTSC\
   (0x00eu)
#define TUX64_BOOT_STAGE1_VIDEO_V_BURST_END_NTSC\
   (0x204u)

#define TUX64_BOOT_STAGE1_VIDEO_X_OFFSET\
   (0u)
#define TUX64_BOOT_STAGE1_VIDEO_X_SCALE\
   (0x400u)
#define TUX64_BOOT_STAGE1_VIDEO_Y_OFFSET\
   (0u)
#define TUX64_BOOT_STAGE1_VIDEO_Y_SCALE\
   (0x400u)

struct Tux64BootStage1VideoContext {
   struct Tux64BootStage1VideoFramebuffer framebuffers [TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT];
   volatile Tux64UInt64 clear_color_rsp_dma_buffer [0x1000u / sizeof(Tux64UInt64)]
   __attribute__((aligned(8u))); /* alignment required for RSP DMA */
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
   const volatile void * address_virtual;
   const volatile void * address_physical;

   /* VI uses the physical address into RDRAM, not the virtual address */
   /* used by the SysAD bus, which is why we do this. */
   address_virtual   = tux64_boot_stage1_video_framebuffer_get(index);
   address_physical  = tux64_platform_mips_n64_memory_map_direct_uncached_to_physical(address_virtual);

   tux64_platform_mips_n64_mmio_registers_vi.origin = (Tux64UInt32)(Tux64UIntPtr)address_physical;

   return;
}

static Tux64UInt64
tux64_boot_stage1_video_clear_color_get_uint64(
   Tux64BootStage1VideoPixel clear_color
) {
   Tux64UInt64 retn;
   Tux64UInt8 i;

   retn = (Tux64UInt64)clear_color;
   i = TUX64_LITERAL_UINT8(sizeof(retn) / sizeof(Tux64BootStage1VideoPixel));

   do {
      retn |= (retn << TUX64_LITERAL_UINT8(sizeof(Tux64BootStage1VideoPixel) * 8u));
      i--;
   } while (i != TUX64_LITERAL_UINT32(0));

   return retn;
}

static void
tux64_boot_stage1_video_initialize_context_clear_color(
   Tux64BootStage1VideoPixel clear_color
) {
   struct Tux64BootStage1VideoContext * ctx;
   Tux64UInt64 clear_color_x8b;
   volatile Tux64UInt64 * iter_dma_buffer;
   Tux64UInt32 words_remaining;

   ctx = &tux64_boot_stage1_video_context;

   /* we do this because it's cheaper to loop less times and issue 64-bit */
   /* stores than 2x/4x as many 32-bit/16-bit stores. */
   clear_color_x8b = tux64_endian_convert_uint64(
      tux64_boot_stage1_video_clear_color_get_uint64(clear_color),
      TUX64_ENDIAN_FORMAT_BIG
   );

   /* we now fill the rsp dma buffer 8 bytes at a time with out pixel data. */
   /* when we later clear the framebuffer, this will be used to fill the */
   /* entire framebuffer one 4kb chunk at a time.  also, we don't cache */
   /* writes because they could be used by the RSP before writeback, also it */
   /* will need to be flushed immediately anyways, which could waste cache */
   /* and unnecessarily writeback legitimately cached data. */
   iter_dma_buffer = (volatile Tux64UInt64 *)tux64_platform_mips_n64_memory_map_direct_cached_to_direct_uncached(ctx->clear_color_rsp_dma_buffer);
   words_remaining = TUX64_LITERAL_UINT32(sizeof(ctx->clear_color_rsp_dma_buffer));
   do {
      *iter_dma_buffer = clear_color_x8b;

      iter_dma_buffer++;
      words_remaining--;
   } while (words_remaining != TUX64_LITERAL_UINT32(0u));

   return;
}

static void
tux64_boot_stage1_video_framebuffer_clear(
   Tux64UInt8 idx
) {
   struct Tux64BootStage1VideoContext * ctx;
   struct Tux64BootStage1VideoFramebuffer * framebuffer;
   Tux64UInt8 blocks;
   Tux64UInt8 bytes_remainder;

   ctx         = &tux64_boot_stage1_video_context;
   framebuffer = tux64_boot_stage1_video_framebuffer_get(idx);

   blocks            = TUX64_LITERAL_UINT8(sizeof(framebuffer->pixels) / sizeof(ctx->clear_color_rsp_dma_buffer));
   bytes_remainder   = TUX64_LITERAL_UINT8(sizeof(framebuffer->pixels) % sizeof(ctx->clear_color_rsp_dma_buffer));

   /* here, we use RSP DMA to efficiently fill the entire framebuffer with a  */
   /* repeating pattern of pixels.  this is the fastest way to fill any chunk */
   /* of memory on the N64, and this is a perfect use case.  visually, this   */
   /* is what we're doing:                                                    */
   /*                                                                         */
   /*         [RDRAM]            [RSP DMA TRANSFER]          [RSP IMEM]       */
   /*                                                                         */
   /* clear_color_rsp_dma_buffer ----- 4KiB ---->        (uninitialized)      */
   /* framebuffer[0]             <---- 4KiB -----  clear_color_rsp_dma_buffer */
   /* framebuffer[1]             <---- 4KiB -----  clear_color_rsp_dma_buffer */
   /*                                  ...                                    */
   /* framebuffer[blocks - 1]    <---- 4KiB -----  clear_color_rsp_dma_buffer */
   /* framebuffer[blocks]        <-- remainder --  clear_color_rsp_dma_buffer */
   /*                                                                         */
   /* here, we use a spinlock after each DMA transfer to make this code       */
   /* synchronous with the RSP.  theoretically, we could use SP interrupts to */
   /* do this asynchronously, but performance in the main loop isn't an issue */
   /* here, and this severely complicates our code, so we don't do this.      */
   /*                                                                         */
   /* also, the reason we use IMEM and not DMEM is so we preserve boot status */
   /* codes (if they're enabled).  otherwise, we either sacrifice on transfer */
   /* size to avoid overwriting the boot status code or we have to write to a */
   /* different location, which is not ideal since now multiple boot stages   */
   /* have multiple status code addresses.  this is also why we're not        */
   /* uploading microcode to the RSP, as that could interfere with boot       */
   /* status codes for the same reason.                                       */

   /* TODO: implement */
   (void)framebuffer;
   (void)blocks;
   (void)bytes_remainder;
   
   return;
}

static void
tux64_boot_stage1_video_initialize_context(
   Tux64BootStage1VideoPixel clear_color
) {
   struct Tux64BootStage1VideoContext * ctx;
   Tux64UInt8 i;

   /* TODO: choose resolution at runtime so we can support both PAL and NTSC */
   /* framebuffers with the same code. */

   /* done to save on typing */
   ctx = &tux64_boot_stage1_video_context;

   tux64_boot_stage1_video_initialize_context_clear_color(clear_color);

   /* clear non-rendering framebuffers to prevent garbage from being */
   /* displayed on startup */
   i = TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT - 1u);
   while (i != TUX64_LITERAL_UINT8(0u)) {
      tux64_boot_stage1_video_framebuffer_clear(i);
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
   Tux64UInt32 pixel_advance;
   Tux64UInt32 burst;
   Tux64UInt32 v_total;
   Tux64UInt32 h_total;
   Tux64UInt32 h_start;
   Tux64UInt32 h_end;
   Tux64UInt32 v_start;
   Tux64UInt32 v_end;
   Tux64UInt32 v_burst_start;
   Tux64UInt32 v_burst_end;

   /* done to save on typing */
   vi = &tux64_platform_mips_n64_mmio_registers_vi;

   /* set platform-specific configurations */
   switch (platform) {
      case TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_PAL:
         pixel_advance  = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_PIXEL_ADVANCE_DEFAULT);
         burst          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_BURST_PAL);
         v_total        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_SCANLINES_PAL);
         h_total        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_TOTAL_PAL);
         h_start        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_START_PAL);
         h_end          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_END_PAL);
         v_start        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_START_PAL);
         v_end          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_END_PAL);
         v_burst_start  = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_BURST_START_PAL);
         v_burst_end    = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_BURST_END_PAL);
         break;
      case TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_NTSC:
         pixel_advance  = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_PIXEL_ADVANCE_DEFAULT);
         burst          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_BURST_NTSC);
         v_total        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_SCANLINES_NTSC);
         h_total        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_TOTAL_NTSC);
         h_start        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_START_NTSC);
         h_end          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_END_NTSC);
         v_start        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_START_NTSC);
         v_end          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_END_NTSC);
         v_burst_start  = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_BURST_START_NTSC);
         v_burst_end    = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_BURST_END_NTSC);
         break;
      case TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_MPAL:
         pixel_advance  = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_PIXEL_ADVANCE_DEFAULT);
         burst          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_BURST_NTSC);
         v_total        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_SCANLINES_NTSC);
         h_total        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_TOTAL_MPAL);
         h_start        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_START_NTSC);
         h_end          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_END_NTSC);
         v_start        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_START_NTSC);
         v_end          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_END_NTSC);
         v_burst_start  = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_BURST_START_NTSC);
         v_burst_end    = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_BURST_END_NTSC);
         break;
      case TUX64_BOOT_STAGE1_VIDEO_PLATFORM_IQUE:
         pixel_advance  = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_PIXEL_ADVANCE_IQUE);
         burst          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_BURST_NTSC);
         v_total        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_SCANLINES_NTSC);
         h_total        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_TOTAL_NTSC);
         h_start        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_START_NTSC);
         h_end          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_H_END_NTSC);
         v_start        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_START_NTSC);
         v_end          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_END_NTSC);
         v_burst_start  = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_BURST_START_NTSC);
         v_burst_end    = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_V_BURST_END_NTSC);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   tux64_boot_stage1_video_set_vi_framebuffer(
      tux64_boot_stage1_video_framebuffer_index_get_displaying()
   );

   vi->width = TUX64_LITERAL_UINT32(
      TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_X
   );

   vi->v_intr = TUX64_LITERAL_UINT32(
      0x00000002u /* trigger interrupt on 2nd line of vblank*/
   );
   vi->burst = burst;

   vi->v_total = v_total - TUX64_LITERAL_UINT32(1u);

   vi->h_total =
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_VI_LEAP_DEFAULT) |
      (h_total - TUX64_LITERAL_UINT32(1u));

   vi->h_video =
      (h_start << TUX64_LITERAL_UINT8(TUX64_PLATFORM_MIPS_N64_VI_H_START_BIT_OFFSET)) |
      (h_end   << TUX64_LITERAL_UINT8(TUX64_PLATFORM_MIPS_N64_VI_H_END_BIT_OFFSET));

   vi->v_video =
      (v_start << TUX64_LITERAL_UINT8(TUX64_PLATFORM_MIPS_N64_VI_V_START_BIT_OFFSET)) |
      (v_end   << TUX64_LITERAL_UINT8(TUX64_PLATFORM_MIPS_N64_VI_V_END_BIT_OFFSET));

   vi->v_burst =
      (v_burst_start << TUX64_LITERAL_UINT8(TUX64_PLATFORM_MIPS_N64_VI_V_BURST_START_BIT_OFFSET)) |
      (v_burst_end   << TUX64_LITERAL_UINT8(TUX64_PLATFORM_MIPS_N64_VI_V_BURST_END_BIT_OFFSET));

   vi->x_scale = TUX64_LITERAL_UINT32(
      TUX64_BOOT_STAGE1_VIDEO_X_OFFSET << TUX64_PLATFORM_MIPS_N64_VI_X_OFFSET_BIT_OFFSET |
      TUX64_BOOT_STAGE1_VIDEO_X_SCALE  << TUX64_PLATFORM_MIPS_N64_VI_X_SCALE_BIT_OFFSET
   );

   vi->y_scale = TUX64_LITERAL_UINT32(
      TUX64_BOOT_STAGE1_VIDEO_Y_OFFSET << TUX64_PLATFORM_MIPS_N64_VI_Y_OFFSET_BIT_OFFSET |
      TUX64_BOOT_STAGE1_VIDEO_Y_SCALE  << TUX64_PLATFORM_MIPS_N64_VI_Y_SCALE_BIT_OFFSET
   );

   /* VI_CTRL should be set last since setting this will enable video output */
   vi->ctrl = TUX64_LITERAL_UINT32(
      (Tux64UInt32)TUX64_PLATFORM_MIPS_N64_VI_DEDITHER_DISABLE |
      (Tux64UInt32)TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_DISABLE |
      (Tux64UInt32)TUX64_PLATFORM_MIPS_N64_VI_SERRATE_DISABLE |
      (Tux64UInt32)TUX64_PLATFORM_MIPS_N64_VI_DIVOT_DISABLE |
      (Tux64UInt32)TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DISABLE |
      (Tux64UInt32)TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DITHER_DISABLE |
      (Tux64UInt32)TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXEL_FORMAT
   ) | (pixel_advance << TUX64_LITERAL_UINT8(TUX64_PLATFORM_MIPS_N64_VI_PIXEL_ADVANCE_BIT_OFFSET));

   return;
}

void
tux64_boot_stage1_video_initialize(
   enum Tux64BootStage1VideoPlatform platform,
   Tux64BootStage1VideoPixel clear_color
) {
   tux64_boot_stage1_video_initialize_context(clear_color);
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
tux64_boot_stage1_video_render_target_get(void) {
   Tux64UInt8 idx;

   idx = tux64_boot_stage1_video_framebuffer_index_get_rendering();

   return tux64_boot_stage1_video_framebuffer_get(idx);
}

void
tux64_boot_stage1_video_render_target_clear(void) {
   Tux64UInt8 idx;

   idx = tux64_boot_stage1_video_framebuffer_index_get_rendering();

   tux64_boot_stage1_video_framebuffer_clear(idx);
   return;
}

