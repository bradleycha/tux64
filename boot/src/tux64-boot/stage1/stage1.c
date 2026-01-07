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
#include "tux64-boot/stage1/format.h"
#include "tux64-boot/stage1/boot-header.h"
#include "tux64-boot/stage1/preempt.h"

static Tux64Boolean
tux64_boot_stage1_checksum_enable(void) {
   if (!TUX64_BOOT_CONFIG_CHECKSUM) {
      return TUX64_BOOLEAN_FALSE;
   }

   return (tux64_boot_stage1_boot_header_flag_no_checksum() == TUX64_BOOLEAN_FALSE);
}

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

struct Tux64BootStage1FsmMemoryTest {
   struct Tux64BootStage1FormatPercentageContext percentage;
   Tux64BootStage1FbconLabel label;
};

union Tux64BootStage1FsmMemory {
   struct Tux64BootStage1FsmMemoryTest test;
};

struct Tux64BootStage1Fsm;

typedef void (*Tux64BootStage1FsmPfnState)(struct Tux64BootStage1Fsm * fsm);

struct Tux64BootStage1Fsm {
   union Tux64BootStage1FsmMemory memory;
   Tux64BootStage1FsmPfnState state;
};

#define TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(identifier) \
   static void identifier (struct Tux64BootStage1Fsm *)
#define TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(identifier) \
   static void identifier (struct Tux64BootStage1Fsm * fsm)

TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_test);

static void
tux64_boot_stage1_fsm_state_set_test(
   struct Tux64BootStage1Fsm * fsm
) {
   struct Tux64BootStage1FsmMemoryTest * mem;

   mem = &fsm->memory.test;

   tux64_boot_stage1_format_percentage_initialize(
      &mem->percentage,
      TUX64_LITERAL_UINT32(30000000u)
   );

   mem->label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_hello_world);
   tux64_boot_stage1_format_percentage(&mem->percentage, mem->label);

   fsm->state = tux64_boot_stage1_fsm_state_test;
   return;
}

TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(tux64_boot_stage1_fsm_state_test) {
   struct Tux64BootStage1FsmMemoryTest * mem;
   volatile Tux64UInt32 value;

   mem = &fsm->memory.test;

   /* volatile so compiler optimizations can't remove the loop */
   value = TUX64_LITERAL_UINT32(1u);

   do {
      tux64_boot_stage1_format_percentage_accumulate(&mem->percentage, value);
   } while (tux64_boot_stage1_preempt_yield() == TUX64_BOOLEAN_FALSE);

   tux64_boot_stage1_format_percentage(&mem->percentage, mem->label);
   return;
}

static void
tux64_boot_stage1_fsm_initialize_splash(void) {
   (void)tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_splash);
   tux64_boot_stage1_fbcon_skip_line();

   return;
}

static void
tux64_boot_stage1_fsm_initialize_memory_display(
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_free
) {
   Tux64BootStage1FbconLabel label;
   Tux64UInt32 kernel_image_words;
   Tux64UInt32 initramfs_image_words;
   Tux64UInt32 kernel_image;
   Tux64UInt32 initramfs_image;

   if (tux64_boot_stage1_boot_header_flag_memory_display() == TUX64_BOOLEAN_FALSE) {
      return;
   }

   kernel_image_words      = tux64_boot_stage1_boot_header.data.files.kernel.length_words;
   initramfs_image_words   = tux64_boot_stage1_boot_header.data.files.initramfs.length_words;

   kernel_image      = kernel_image_words    * TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD);
   initramfs_image   = initramfs_image_words * TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD);

   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_memory_total);
   tux64_boot_stage1_format_mib(label, memory_total);
   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_memory_free);
   tux64_boot_stage1_format_mib(label, memory_free);
   tux64_boot_stage1_fbcon_skip_line();

   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_kernel_image);
   tux64_boot_stage1_format_mib(label, kernel_image);
   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_initramfs_image);
   tux64_boot_stage1_format_mib(label, initramfs_image);
   tux64_boot_stage1_fbcon_skip_line();

   return;
}

static void
tux64_boot_stage1_fsm_initialize_checksum(void) {
   if (!tux64_boot_stage1_checksum_enable()) {
      (void)tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_no_checksum);
      return;
   }

   return;
}

static void
tux64_boot_stage1_fsm_initialize(
   struct Tux64BootStage1Fsm * fsm,
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_free
) {
   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_INITIAL);

   if (TUX64_BOOT_CONFIG_SPLASH) {
      tux64_boot_stage1_fsm_initialize_splash();
   }

   if (TUX64_BOOT_CONFIG_MEMORY_DISPLAY) {
      tux64_boot_stage1_fsm_initialize_memory_display(memory_total, memory_free);
   }

   if (TUX64_BOOT_CONFIG_CHECKSUM) {
      tux64_boot_stage1_fsm_initialize_checksum();
   }

   tux64_boot_stage1_fsm_state_set_test(fsm);
   return;
}

static void
tux64_boot_stage1_fsm_execute(
   struct Tux64BootStage1Fsm * fsm
) {
   fsm->state(fsm);
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
   Tux64UInt32 memory_free,
   Tux64Boolean running_on_ique
) {
   enum Tux64BootStage1VideoPlatform video_platform;
   struct Tux64BootStage1Palette video_palette;
   struct Tux64BootStage1Fsm fsm;

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

   tux64_boot_stage1_fsm_initialize(
      &fsm,
      memory_total,
      memory_free
   );

   while (TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_video_render_target_clear();
      tux64_boot_stage1_fsm_execute(&fsm);
      tux64_boot_stage1_fbcon_render();
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

