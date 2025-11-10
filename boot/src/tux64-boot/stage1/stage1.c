/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.c - Stage-1 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/ipl2.h"
#include "tux64-boot/stage1/status.h"
#include "tux64-boot/stage1/interrupt/interrupt.h"
#include "tux64-boot/stage1/video.h"

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

struct Tux64BootStage1MainLoopContextMemory {
   Tux64UInt32 total;
   Tux64UInt32 available;
};

struct Tux64BootStage1MainLoopContext {
   struct Tux64BootStage1MainLoopContextMemory memory;
};

static void
tux64_boot_stage1_main_loop_context_initialize(
   struct Tux64BootStage1MainLoopContext * context,
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_available
) {
   context->memory.total = memory_total;
   context->memory.available = memory_available;
   return;
}

static void
tux64_boot_stage1_main_loop_context_execute(
   struct Tux64BootStage1MainLoopContext * context
) {
   /* TODO: implement */
   (void)context;
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
   struct Tux64BootStage1MainLoopContext main_loop_context;

   tux64_boot_stage1_status_initialize();

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_BEGIN);

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_INTERRUPT);
   tux64_boot_stage1_interrupt_initialize();

   video_platform = tux64_boot_stage1_choose_video_platform(
      video_standard,
      running_on_ique
   );

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_INITIALIZE_VIDEO);
   tux64_boot_stage1_interrupt_vi_disable();
   tux64_boot_stage1_video_initialize(
      video_platform,
      TUX64_BOOT_STAGE1_VIDEO_CLEAR_COLOR_SAPPHIRE /* a lovely blue color to stare at until we make this configurable! */
   );
   tux64_boot_stage1_interrupt_vi_enable();

   tux64_boot_stage1_main_loop_context_initialize(
      &main_loop_context,
      memory_total,
      memory_available
   );
   
   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_LOOP);
   while (TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_video_render_target_clear();
      tux64_boot_stage1_main_loop_context_execute(&main_loop_context);
      tux64_boot_stage1_video_swap_buffers();
   }

   (void)rom_type;
   (void)reset_type;
   (void)rom_cic_seed;
   (void)pif_rom_version;
   TUX64_UNREACHABLE;
}

