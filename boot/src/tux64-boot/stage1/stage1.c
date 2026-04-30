/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.c - Stage-1 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/stage1.h"

#include "tux64-boot/initialize.h"
#include "tux64-boot/ipl2.h"
#include "tux64-boot/rsp.h"
#include "tux64-boot/stage1/status.h"
#include "tux64-boot/stage1/memory.h"
#include "tux64-boot/stage1/interrupt.h"
#include "tux64-boot/stage1/video.h"
#include "tux64-boot/stage1/fbcon.h"
#include "tux64-boot/stage1/logo.h"
#include "tux64-boot/stage1/fsm.h"

enum Tux64BootStage1Color {
   TUX64_BOOT_STAGE1_COLOR_BLACK    = 0x0001u,
   TUX64_BOOT_STAGE1_COLOR_WHITE    = 0xffffu,
   TUX64_BOOT_STAGE1_COLOR_GRAY     = 0x8421u,
   TUX64_BOOT_STAGE1_COLOR_MAGENTA  = 0xd829u,
   TUX64_BOOT_STAGE1_COLOR_RED      = 0xf801u,
   TUX64_BOOT_STAGE1_COLOR_ORANGE   = 0xfbc1u,
   TUX64_BOOT_STAGE1_COLOR_YELLOW   = 0xffc1u,
   TUX64_BOOT_STAGE1_COLOR_GREEN    = 0x07c1u,
   TUX64_BOOT_STAGE1_COLOR_CYAN     = 0x0529u,
   TUX64_BOOT_STAGE1_COLOR_BLUE     = 0x003fu,
   TUX64_BOOT_STAGE1_COLOR_INDIGO   = 0x2823u,
   TUX64_BOOT_STAGE1_COLOR_VIOLET   = 0x8835u,
   TUX64_BOOT_STAGE1_COLOR_PURPLE   = 0x5029u,
   TUX64_BOOT_STAGE1_COLOR_RUBY     = 0x4801u,
   TUX64_BOOT_STAGE1_COLOR_EMERALD  = 0x0241u,
   TUX64_BOOT_STAGE1_COLOR_SAPPHIRE = 0x0013u
};

struct Tux64BootStage1Palette {
   enum Tux64BootStage1Color foreground;
   enum Tux64BootStage1Color background;
};

static struct Tux64BootStage1Palette
tux64_boot_stage1_choose_video_palette(void) {
   struct Tux64BootStage1Palette palette;

   palette.foreground = TUX64_BOOT_CONFIG_COLOR_FOREGROUND;
   palette.background = TUX64_BOOT_CONFIG_COLOR_BACKGROUND;
   return palette;
}

static enum Tux64BootStage1VideoPlatform
tux64_boot_stage1_choose_video_platform(
   enum Tux64BootIpl2VideoStandard video_standard,
   Tux64Boolean running_on_ique
) {
   if (running_on_ique == TUX64_BOOLEAN_TRUE) {
      return TUX64_BOOT_STAGE1_VIDEO_PLATFORM_IQUE;
   }

   return (enum Tux64BootStage1VideoPlatform)video_standard;
}

/* we store the FSM in a global variable because its state can be huge (think */
/* DMA buffers).  if we try to store it on the stack, it will overflow 100%   */
/* and crash our program.  storing in .bss via a global variable doesn't have */
/* this issue. */
static struct Tux64BootStage1Fsm
tux64_boot_stage1_fsm;

void
tux64_boot_stage1_main(
   enum Tux64BootIpl2RomType rom_type,
   enum Tux64BootIpl2VideoStandard video_standard,
   enum Tux64BootIpl2ResetType reset_type,
   Tux64UInt8 rom_cic_seed,
   Tux64UInt8 pif_rom_version,
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_free,
   Tux64Boolean running_on_ique
) {
   enum Tux64BootStage1VideoPlatform video_platform;
   struct Tux64BootStage1Palette video_palette;

   tux64_boot_initialize();

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_BEGIN);

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_MEMORY);
   tux64_boot_stage1_memory_initialize(memory_total, memory_free);

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_INTERRUPT);
   tux64_boot_stage1_interrupt_initialize();

   video_palette = tux64_boot_stage1_choose_video_palette();

   video_platform = tux64_boot_stage1_choose_video_platform(
      video_standard,
      running_on_ique
   );

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_VIDEO);
   tux64_boot_stage1_interrupt_vi_disable();
   tux64_boot_stage1_video_initialize(
      video_platform,
      video_palette.background
   );
   tux64_boot_stage1_interrupt_vi_enable();

   if (TUX64_BOOT_CONFIG_LOGO) {
      tux64_boot_stage1_logo_initialize(video_palette.background);
   }

   tux64_boot_stage1_fbcon_initialize(
      video_palette.foreground,
      video_palette.background
   );

   tux64_boot_stage1_fsm_initialize(&tux64_boot_stage1_fsm);

   while (TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_fsm_execute(&tux64_boot_stage1_fsm);

      /* we order all rendering code next to each other not just because   */
      /* it makes sense, but we can also potentially keep the RSP DMA      */
      /* engine saturated with work as a previous DMA transfer completes.  */
      tux64_boot_stage1_video_render_target_clear();
      tux64_boot_rsp_dma_wait_queue();
      tux64_boot_stage1_fbcon_render();
      if (TUX64_BOOT_CONFIG_LOGO) {
         tux64_boot_rsp_dma_wait_queue();
         tux64_boot_stage1_logo_render();
      }

      /* synchronize all RSP DMA operations, otherwise we could end up     */
      /* swapping buffers in the middle of rendering if the FSM state was  */
      /* preempted by vblank, thus we swap buffers immediately.  this also */
      /* ensures correct synchronization for the start of the next render. */
      tux64_boot_rsp_dma_wait_idle();

      /* this only works when we take less than one VI to execute, i.e. we */
      /* don't have a lag frame.  our bootloader is so simple, we have     */
      /* full control and can write code that never lags.  that is, the    */
      /* solution is to just program well, forehead.                       */
      tux64_boot_stage1_video_vblank_wait();
      tux64_boot_stage1_video_swap_buffers();
      tux64_boot_stage1_video_vblank_end();
   }

   (void)rom_type;
   (void)reset_type;
   (void)rom_cic_seed;
   (void)pif_rom_version;
   TUX64_UNREACHABLE;
}

