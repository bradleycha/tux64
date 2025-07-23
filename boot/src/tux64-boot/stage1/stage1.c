/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.c - Stage-1 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/ipl2.h"
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

static void
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

   tux64_boot_stage1_interrupt_initialize();

   video_platform = tux64_boot_stage1_choose_video_platform(
      video_standard,
      running_on_ique
   );

   tux64_boot_stage1_interrupt_vi_disable();
   tux64_boot_stage1_video_initialize(video_platform);
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

   (void)rom_type;
   (void)reset_type;
   (void)rom_cic_seed;
   (void)pif_rom_version;
   TUX64_UNREACHABLE;
}

void
tux64_boot_stage1_start(void)
__attribute__((noreturn, section(".start"), externally_visible));

void
tux64_boot_stage1_start(void) {
   /* we do this so we can read parameters from both IPL2 and stage-0 without */
   /* having to either move registers around or use a custom calling */
   /* convention. */
   register Tux64UInt32 rom_type          __asm__("$s3");
   register Tux64UInt32 video_standard    __asm__("$s4");
   register Tux64UInt32 reset_type        __asm__("$s5");
   register Tux64UInt8  rom_cic_seed      __asm__("$s6");
   register Tux64UInt8  pif_rom_version   __asm__("$s7");
   register Tux64UInt32 memory_total      __asm__("$a0");
   register Tux64UInt32 memory_available  __asm__("$a1");
   register Tux64UInt32 running_on_ique   __asm__("$a2");

   /* we have to initialize $gp for use with small data sections.  "_gp" is */
   /* defined by the linker. */
   __asm__ volatile (
      "lui $gp,%hi(_gp)\n"
      "addiu $gp,$gp,%lo(_gp)\n"
   );

   tux64_boot_stage1_main(
      (enum Tux64BootIpl2RomType)rom_type,
      (enum Tux64BootIpl2VideoStandard)video_standard,
      (enum Tux64BootIpl2ResetType)reset_type,
      rom_cic_seed,
      pif_rom_version,
      memory_total,
      memory_available,
      running_on_ique == TUX64_LITERAL_UINT32(0u)
         ? TUX64_BOOLEAN_FALSE
         : TUX64_BOOLEAN_TRUE
   );

   TUX64_UNREACHABLE;
}

