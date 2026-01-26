/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
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
#include "tux64-boot/halt.h"
#include "tux64-boot/sync.h"
#include "tux64-boot/rsp.h"
#include "tux64-boot/stage1/interrupt.h"

#define TUX64_BOOT_STAGE1_VIDEO_UNKNOWN_CONFIGURATION \
   !( \
      TUX64_BOOT_CONFIG_REGION_PAL  || \
      TUX64_BOOT_CONFIG_REGION_NTSC || \
      TUX64_BOOT_CONFIG_REGION_MPAL || \
      TUX64_BOOT_CONFIG_IQUE \
   )

/* we need this to prevent misconfigurations from exploding at runtime. */
#if TUX64_BOOT_STAGE1_VIDEO_UNKNOWN_CONFIGURATION
#error unable to determine video configurations parameters.  please reconfigure with at least one region supported with --enable-region-[region], or enable iQue player support with --enable-ique
#endif /* TUX64_BOOT_STAGE1_VIDEO_UNKNOWN_CONFIGURATION */

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
   struct Tux64BootSyncFence vblank_fence;
   Tux64UInt8 framebuffer_index_displaying;
};

static struct Tux64BootStage1VideoContext
tux64_boot_stage1_video_context;

static struct Tux64BootStage1VideoFramebuffer *
tux64_boot_stage1_video_framebuffer_get(
   Tux64UInt8 idx
) {
   return &tux64_boot_stage1_video_context.framebuffers[idx];
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
   const volatile void * address;

   /* the upper 8 bits of the address are ignored, so we can safely write the */
   /* virtual address without having to convert to a physical address. */
   address = tux64_boot_stage1_video_framebuffer_get(index);
   tux64_platform_mips_n64_mmio_registers_vi.origin = (Tux64UInt32)(Tux64UIntPtr)address;

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
   Tux64UInt16 bytes_remainder;
   struct Tux64BootRspDmaTransfer rsp_dma_transfer;

   ctx         = &tux64_boot_stage1_video_context;
   framebuffer = tux64_boot_stage1_video_framebuffer_get(idx);

   blocks            = TUX64_LITERAL_UINT8(sizeof(framebuffer->pixels) / sizeof(ctx->clear_color_rsp_dma_buffer));
   bytes_remainder   = TUX64_LITERAL_UINT16(sizeof(framebuffer->pixels) % sizeof(ctx->clear_color_rsp_dma_buffer));

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

   /* initialize transfer values which will be shared across transfers.  note */
   /* that we have to split this as 2 "rows", since we can't store 4KiB */
   /* inside a 12-bit integer. */
   rsp_dma_transfer.addr_rsp_mem    = TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RSP_IMEM);
   rsp_dma_transfer.row_bytes_copy  = TUX64_LITERAL_UINT16((sizeof(ctx->clear_color_rsp_dma_buffer) / 2u) - 1u);
   rsp_dma_transfer.row_bytes_skip  = TUX64_LITERAL_UINT16(0u);
   rsp_dma_transfer.row_count       = TUX64_LITERAL_UINT8(1u);

   /* copy the color fill buffer to RSP IMEM */
   rsp_dma_transfer.addr_rdram = (Tux64UInt32)(Tux64UIntPtr)(&ctx->clear_color_rsp_dma_buffer);
   tux64_boot_rsp_dma_start(&rsp_dma_transfer, TUX64_BOOT_RSP_DMA_DESTINATION_RSP_MEMORY);

   /* start filling the framebuffer with the clear color */
   rsp_dma_transfer.addr_rdram = (Tux64UInt32)(Tux64UIntPtr)framebuffer;
   do {
      tux64_boot_rsp_dma_wait_queue();
      tux64_boot_rsp_dma_start(&rsp_dma_transfer, TUX64_BOOT_RSP_DMA_DESTINATION_RDRAM);
      rsp_dma_transfer.addr_rdram += TUX64_LITERAL_UINT32(sizeof(ctx->clear_color_rsp_dma_buffer));
      blocks--;
   } while (blocks != TUX64_LITERAL_UINT32(0u));

   /* fill the remainder bytes */
   if (bytes_remainder != TUX64_LITERAL_UINT32(0u)) {
      rsp_dma_transfer.row_bytes_copy  = ((Tux64UInt16)bytes_remainder - TUX64_LITERAL_UINT16(1u));
      rsp_dma_transfer.row_count       = TUX64_LITERAL_UINT8(0u);

      tux64_boot_rsp_dma_wait_queue();
      tux64_boot_rsp_dma_start(&rsp_dma_transfer, TUX64_BOOT_RSP_DMA_DESTINATION_RDRAM);
   }

   /* wait for the RSP to finish its last copy before we return */
   tux64_boot_rsp_dma_wait_idle();
   
   return;
}

static void
tux64_boot_stage1_video_initialize_context(
   Tux64BootStage1VideoPixel clear_color
) {
   struct Tux64BootStage1VideoContext * ctx;
   Tux64UInt8 i;

   /* done to save on typing */
   ctx = &tux64_boot_stage1_video_context;

   tux64_boot_stage1_video_initialize_context_clear_color(clear_color);
   tux64_boot_sync_fence_initialize(&ctx->vblank_fence);

   /* clear non-rendering framebuffers to prevent garbage from being */
   /* displayed on startup */
   i = TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT - 1u);
   while (i != TUX64_LITERAL_UINT8(0u)) {
      tux64_boot_stage1_video_framebuffer_clear(i);
      i--;
   }

   ctx->framebuffer_index_displaying = TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_VIDEO_CONTEXT_FRAMEBUFFERS_COUNT - 1u);

   return;
}

/* basically a duplicate definition of the usual tux64-lib MMIO struct, but */
/* without VI_TEST_ADDR and VI_STAGED_DATA, which don't need to be written */
/* for screen initialization. */
struct Tux64BootStage1VideoViRegisterArrayRegisters {
   Tux64UInt32 ctrl;
   Tux64UInt32 origin;
   Tux64UInt32 width;
   Tux64UInt32 v_intr;
   Tux64UInt32 v_current;
   Tux64UInt32 burst;
   Tux64UInt32 v_total;
   Tux64UInt32 h_total;
   Tux64UInt32 h_total_leap;
   Tux64UInt32 h_video;
   Tux64UInt32 v_video;
   Tux64UInt32 v_burst;
   Tux64UInt32 x_scale;
   Tux64UInt32 y_scale;
};

/* this is used to more efficiently initialize the VI while also trying to */
/* prevent the code from being too undecipherable.  instead of having to */
/* manually set each individual register, we can just iterate through memory. */
/* to give credit where credit's due, i got this idea from clbr's original */
/* n64linux code in linux/arch/mips/n64/init.c, where he does the same thing, */
/* just with a lot more magic number and a lot less documentation. */
union Tux64BootStage1VideoViRegisterArray {
   struct Tux64BootStage1VideoViRegisterArrayRegisters regs;
   Tux64UInt32 words [sizeof(struct Tux64BootStage1VideoViRegisterArrayRegisters) / sizeof(Tux64UInt32)];
};

#define TUX64_BOOT_STAGE1_VIDEO_VI_CTRL_BASE \
   (\
      TUX64_PLATFORM_MIPS_N64_VI_DEDITHER_DISABLE | \
      TUX64_PLATFORM_MIPS_N64_VI_AA_MODE_DISABLE | \
      TUX64_PLATFORM_MIPS_N64_VI_SERRATE_DISABLE | \
      TUX64_PLATFORM_MIPS_N64_VI_DIVOT_DISABLE | \
      TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DISABLE | \
      TUX64_PLATFORM_MIPS_N64_VI_GAMMA_DITHER_DISABLE | \
      TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXEL_FORMAT \
   )

#define TUX64_BOOT_STAGE1_VIDEO_VI_CTRL_STANDARD \
   ( \
     TUX64_BOOT_STAGE1_VIDEO_VI_CTRL_BASE | \
     (0x3u << TUX64_PLATFORM_MIPS_N64_VI_PIXEL_ADVANCE_BIT_OFFSET) \
   )

#define TUX64_BOOT_STAGE1_VIDEO_VI_CTRL_IQUE \
   ( \
     TUX64_BOOT_STAGE1_VIDEO_VI_CTRL_BASE | \
     (0x1u << TUX64_PLATFORM_MIPS_N64_VI_PIXEL_ADVANCE_BIT_OFFSET) \
   )

#define TUX64_BOOT_STAGE1_VIDEO_VI_X_SCALE \
   ( \
      (0u      << TUX64_PLATFORM_MIPS_N64_VI_X_OFFSET_BIT_OFFSET) | \
      (0x200u  << TUX64_PLATFORM_MIPS_N64_VI_X_SCALE_BIT_OFFSET) \
   )

#define TUX64_BOOT_STAGE1_VIDEO_VI_Y_SCALE \
   ( \
      (0u      << TUX64_PLATFORM_MIPS_N64_VI_Y_OFFSET_BIT_OFFSET) | \
      (0x400u  << TUX64_PLATFORM_MIPS_N64_VI_Y_SCALE_BIT_OFFSET) \
   )

#define TUX64_BOOT_STAGE1_VIDOE_VI_INTR \
   (2u)

#define TUX64_BOOT_STAGE1_VIDEO_VI_V_CURRENT \
   (0u)

/* we have a bit of duplication, but the code required to iterate seperately */
/* and avoid duplication outweighs the benefits. */

#if TUX64_BOOT_CONFIG_REGION_PAL
static const union Tux64BootStage1VideoViRegisterArray
tux64_boot_stage1_video_vi_register_array_pal = {.regs = {
   .ctrl          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_CTRL_STANDARD),
   .origin        = TUX64_LITERAL_UINT32(0x00000000u),
   .width         = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_X),
   .v_intr        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDOE_VI_INTR),
   .v_current     = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_V_CURRENT),
   .burst         = TUX64_LITERAL_UINT32(0x0404233au),
   .v_total       = TUX64_LITERAL_UINT32(625u),
   .h_total       = TUX64_LITERAL_UINT32(3177u),
   .h_total_leap  = TUX64_LITERAL_UINT32(0x0c6f0c6eu),
   .h_video       = TUX64_LITERAL_UINT32(0x00800300u),
   .v_video       = TUX64_LITERAL_UINT32(0x005f0239u),
   .v_burst       = TUX64_LITERAL_UINT32(0x0009026bu),
   .x_scale       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_X_SCALE),
   .y_scale       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_Y_SCALE)
}};
#endif /* TUX64_BOOT_CONFIG_REGION_PAL */

#if TUX64_BOOT_CONFIG_REGION_NTSC
static const union Tux64BootStage1VideoViRegisterArray
tux64_boot_stage1_video_vi_register_array_ntsc = {.regs = {
   .ctrl          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_CTRL_STANDARD),
   .origin        = TUX64_LITERAL_UINT32(0x00000000u),
   .width         = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_X),
   .v_intr        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDOE_VI_INTR),
   .v_current     = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_V_CURRENT),
   .burst         = TUX64_LITERAL_UINT32(0x03e52239u),
   .v_total       = TUX64_LITERAL_UINT32(525u),
   .h_total       = TUX64_LITERAL_UINT32(3093u),
   .h_total_leap  = TUX64_LITERAL_UINT32(0x0c150c15u),
   .h_video       = TUX64_LITERAL_UINT32(0x006c02ecu),
   .v_video       = TUX64_LITERAL_UINT32(0x002501ffu),
   .v_burst       = TUX64_LITERAL_UINT32(0x000e0204u),
   .x_scale       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_X_SCALE),
   .y_scale       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_Y_SCALE)
}};
#endif /* TUX64_BOOT_CONFIG_REGION_NTSC */

/* TODO: make sure all below constants are correct for MPAL */

#if TUX64_BOOT_CONFIG_REGION_MPAL
static const union Tux64BootStage1VideoViRegisterArray
tux64_boot_stage1_video_vi_register_array_mpal = {.regs = {
   .ctrl          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_CTRL_STANDARD),
   .origin        = TUX64_LITERAL_UINT32(0x00000000u),
   .width         = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_X),
   .v_intr        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDOE_VI_INTR),
   .v_current     = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_V_CURRENT),
   .burst         = TUX64_LITERAL_UINT32(0x0404233au), /* ??? */
   .v_total       = TUX64_LITERAL_UINT32(525u),
   .h_total       = TUX64_LITERAL_UINT32(3090u),
   .h_total_leap  = TUX64_LITERAL_UINT32(0x0c150c15u), /* ??? */
   .h_video       = TUX64_LITERAL_UINT32(0x006c02ecu), /* ??? */
   .v_video       = TUX64_LITERAL_UINT32(0x002501ffu), /* ??? */
   .v_burst       = TUX64_LITERAL_UINT32(0x000e0204u), /* ??? */
   .x_scale       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_X_SCALE),
   .y_scale       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_Y_SCALE)
}};
#endif /* TUX64_BOOT_CONFIG_REGION_MPAL */

#if TUX64_BOOT_CONFIG_IQUE
static const union Tux64BootStage1VideoViRegisterArray
tux64_boot_stage1_video_vi_register_array_ique = {.regs = {
   .ctrl          = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_CTRL_IQUE),
   .origin        = TUX64_LITERAL_UINT32(0x00000000u),
   .width         = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_FRAMEBUFFER_PIXELS_X),
   .v_intr        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDOE_VI_INTR),
   .v_current     = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_V_CURRENT),
   .burst         = TUX64_LITERAL_UINT32(0x03e52239u),
   .v_total       = TUX64_LITERAL_UINT32(525u),
   .h_total       = TUX64_LITERAL_UINT32(3093u),
   .h_total_leap  = TUX64_LITERAL_UINT32(0x0c150c15u),
   .h_video       = TUX64_LITERAL_UINT32(0x006c02ecu),
   .v_video       = TUX64_LITERAL_UINT32(0x002501ffu),
   .v_burst       = TUX64_LITERAL_UINT32(0x000e0204u),
   .x_scale       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_X_SCALE),
   .y_scale       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_VIDEO_VI_Y_SCALE)
}};
#endif /* TUX64_BOOT_CONFIG_IQUE */

static const union Tux64BootStage1VideoViRegisterArray *
tux64_boot_stage1_video_vi_choose_register_array(
   enum Tux64BootStage1VideoPlatform platform
) {
   switch (platform) {
      case TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_PAL:
#if TUX64_BOOT_CONFIG_REGION_PAL
         return &tux64_boot_stage1_video_vi_register_array_pal;
#else /* TUX64_BOOT_CONFIG_REGION_PAL */
         break;
#endif /* TUX64_BOOT_CONFIG_REGION_PAL */

      case TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_NTSC:
#if TUX64_BOOT_CONFIG_REGION_NTSC
         return &tux64_boot_stage1_video_vi_register_array_ntsc;
#else /* TUX64_BOOT_CONFIG_REGION_NTSC */
         break;
#endif /* TUX64_BOOT_CONFIG_REGION_NTSC */

      case TUX64_BOOT_STAGE1_VIDEO_PLATFORM_N64_MPAL:
#if TUX64_BOOT_CONFIG_REGION_MPAL
         return &tux64_boot_stage1_video_vi_register_array_mpal;
#else /* TUX64_BOOT_CONFIG_REGION_MPAL */
         break;
#endif /* TUX64_BOOT_CONFIG_REGION_MPAL */

      case TUX64_BOOT_STAGE1_VIDEO_PLATFORM_IQUE:
#if TUX64_BOOT_CONFIG_IQUE
         return &tux64_boot_stage1_video_vi_register_array_ique;
#else /* TUX64_BOOT_CONFIG_IQUE */
         break;
#endif /* TUX64_BOOT_CONFIG_IQUE */

      default:
         TUX64_UNREACHABLE;
   }

   /* if we reach here, it's because we got a video configuration we don't */
   /* support, and thus the only safe option is to halt. */
   tux64_boot_halt();
}

static void
tux64_boot_stage1_video_initialize_vi(
   enum Tux64BootStage1VideoPlatform platform
) {
   const union Tux64BootStage1VideoViRegisterArray * register_array;
   const Tux64UInt32 * iter_register_words;
   volatile Tux64UInt32 * iter_vi;
   Tux64UInt8 i;

   register_array = tux64_boot_stage1_video_vi_choose_register_array(platform);

   iter_register_words = register_array->words;
   iter_vi = (volatile Tux64UInt32 *)&tux64_platform_mips_n64_mmio_registers_vi;

   /* we iterate backwards so that VI_CTRL is the last register we set, which */
   /* is what enables video output and starts drawing pixels to the screen. */
   iter_register_words  += TUX64_ARRAY_ELEMENTS(register_array->words);
   iter_vi              += TUX64_ARRAY_ELEMENTS(register_array->words);

   i = TUX64_LITERAL_UINT8(TUX64_ARRAY_ELEMENTS(register_array->words));
   do {
      iter_register_words--;
      iter_vi--;
      i--;

      *iter_vi = *iter_register_words;
   } while (i != TUX64_LITERAL_UINT8(0u));

   return;
}

void
tux64_boot_stage1_video_initialize(
   enum Tux64BootStage1VideoPlatform platform,
   Tux64BootStage1VideoPixel clear_color
) {
   /* must initialize VI first, so that the context initialization can set */
   /* the correct framebuffer.  otherwise, the first frame displayed will be */
   /* garbage. */
   tux64_boot_stage1_video_initialize_vi(platform);
   tux64_boot_stage1_video_initialize_context(clear_color);
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
tux64_boot_stage1_video_vblank_wait(void) {
   struct Tux64BootStage1VideoContext * ctx;

   ctx = &tux64_boot_stage1_video_context;

   tux64_boot_sync_fence_wait(&ctx->vblank_fence);
   return;
}

void
tux64_boot_stage1_video_vblank_end(void) {
   struct Tux64BootStage1VideoContext * ctx;

   ctx = &tux64_boot_stage1_video_context;

   tux64_boot_sync_fence_initialize(&ctx->vblank_fence);
   return;
}

Tux64Boolean
tux64_boot_stage1_video_vblank_triggered(void) {
   struct Tux64BootStage1VideoContext * ctx;

   ctx = &tux64_boot_stage1_video_context;

   return tux64_boot_sync_fence_is_signaled(&ctx->vblank_fence); 
}

void
tux64_boot_stage1_video_vblank_handler(void) {
   struct Tux64BootStage1VideoContext * ctx;

   ctx = &tux64_boot_stage1_video_context;

   tux64_boot_sync_fence_signal(&ctx->vblank_fence);
   return;
}

void
tux64_boot_stage1_video_swap_buffers(void) {
   struct Tux64BootStage1VideoContext * ctx;
   Tux64UInt8 idx_rendering;

   ctx = &tux64_boot_stage1_video_context;

   idx_rendering = tux64_boot_stage1_video_framebuffer_index_get_rendering();
   ctx->framebuffer_index_displaying = idx_rendering;
   tux64_boot_stage1_video_set_vi_framebuffer(idx_rendering);

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

