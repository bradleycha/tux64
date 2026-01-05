/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.c - Stage-1 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/ipl2.h"
#include "tux64-boot/stage1/status.h"
#include "tux64-boot/stage1/interrupt/interrupt.h"
#include "tux64-boot/stage1/video.h"
#include "tux64-boot/stage1/fbcon.h"
#include "tux64-boot/stage1/strings.h"
#include "tux64-boot/stage1/percentage.h"

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
#if TUX64_BOOT_CONFIG_IQUE
   if (running_on_ique == TUX64_BOOLEAN_TRUE) {
      return TUX64_BOOT_STAGE1_VIDEO_PLATFORM_IQUE;
   }
#else /* TUX64_BOOT_CONFIG_IQUE */
   (void)running_on_ique;
#endif /* TUX64_BOOT_CONFIG_IQUE */

   return (enum Tux64BootStage1VideoPlatform)video_standard;
}

struct Tux64BootStage1MainLoopContextMemory {
   Tux64UInt32 total;
   Tux64UInt32 available;
};

struct Tux64BootStage1MainLoopContext {
   struct Tux64BootStage1MainLoopContextMemory memory;
   struct Tux64BootStage1PercentageContext percentage_test;
   Tux64BootStage1FbconLabel label_test;
};

static void
tux64_boot_stage1_main_loop_context_initialize(
   struct Tux64BootStage1MainLoopContext * context,
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_available
) {
   context->memory.total = memory_total;
   context->memory.available = memory_available;

   (void)tux64_boot_stage1_fbcon_label_push(tux64_boot_stage1_strings_splash);
   tux64_boot_stage1_fbcon_skip_line();

   tux64_boot_stage1_percentage_initialize(
      &context->percentage_test,
      TUX64_LITERAL_UINT32(600u)
   );
   context->label_test = tux64_boot_stage1_fbcon_label_push(tux64_boot_stage1_strings_hello_world);

   return;
}

static void
tux64_boot_stage1_main_loop_context_execute(
   struct Tux64BootStage1MainLoopContext * context
) {
   /* test code for percentage */
   tux64_boot_stage1_percentage_format(
      &context->percentage_test,
      context->label_test
   );
   tux64_boot_stage1_percentage_accumulate(
      &context->percentage_test,
      TUX64_LITERAL_UINT32(1u)
   );

   return;
}

void
tux64_boot_stage1_main(
   enum Tux64BootIpl2RomType rom_type,
   enum Tux64BootIpl2VideoStandard video_standard,
   enum Tux64BootIpl2ResetType reset_type,
   Tux64UInt8 rom_cic_seed,
   Tux64UInt8 pif_rom_version,
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_available,
   Tux64Boolean running_on_ique
) {
   enum Tux64BootStage1VideoPlatform video_platform;
   struct Tux64BootStage1Palette video_palette;
   struct Tux64BootStage1MainLoopContext main_loop_context;

   tux64_boot_stage1_status_initialize();

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_BEGIN);

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

   tux64_boot_stage1_fbcon_initialize(
      video_palette.foreground,
      video_palette.background
   );

   tux64_boot_stage1_main_loop_context_initialize(
      &main_loop_context,
      memory_total,
      memory_available
   );

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_LOOP);
   while (TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_video_render_target_clear();
      tux64_boot_stage1_main_loop_context_execute(&main_loop_context);
      tux64_boot_stage1_fbcon_render();
      tux64_boot_stage1_video_swap_buffers();
   }

   (void)rom_type;
   (void)reset_type;
   (void)rom_cic_seed;
   (void)pif_rom_version;
   TUX64_UNREACHABLE;
}

