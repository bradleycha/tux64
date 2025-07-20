/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.c - Stage-1 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/interrupt/interrupt.h"
#include "tux64-boot/stage1/video.h"

static enum Tux64BootStage1VideoPlatform
tux64_boot_stage1_choose_video_platform(
   Tux64UInt32 video_standard,
   Tux64Boolean running_on_ique
) {
   if (running_on_ique) {
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
tux64_boot_stage1_start(
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_available,
   Tux64UInt32 video_standard,
   Tux64Boolean running_on_ique
)
__attribute__((noreturn, section(".start"), externally_visible));

void
tux64_boot_stage1_start(
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_available,
   Tux64UInt32 video_standard,
   Tux64Boolean running_on_ique
) {
   enum Tux64BootStage1VideoPlatform video_platform;
   struct Tux64BootStage1MainLoopContext main_loop_context;

   tux64_boot_stage1_interrupt_initialize();

   video_platform = tux64_boot_stage1_choose_video_platform(
      video_standard,
      running_on_ique
   );

   tux64_boot_stage1_interrupt_vi_disable();
   tux64_boot_stage1_video_initialize(video_platform);
   tux64_boot_stage1_video_display_output(TUX64_BOOLEAN_TRUE);
   tux64_boot_stage1_interrupt_vi_enable();

   tux64_boot_stage1_main_loop_context_initialize(
      &main_loop_context,
      memory_total,
      memory_available
   );

   while (TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_main_loop_context_execute(&main_loop_context);
      tux64_boot_stage1_video_swap_buffers();
   }

   TUX64_UNREACHABLE;
}

