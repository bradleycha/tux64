/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/fsm.c - Implementations for main finite state   */
/*    machine.                                                                */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/fsm.h"

#include <tux64/platform/mips/n64/boot.h>
#include "tux64-boot/stage1/status.h"
#include "tux64-boot/stage1/memory.h"
#include "tux64-boot/stage1/preempt.h"
#include "tux64-boot/stage1/boot-header.h"
#include "tux64-boot/stage1/fbcon.h"
#include "tux64-boot/stage1/strings.h"
#include "tux64-boot/stage1/format.h"

#define TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(identifier) \
   static void identifier (struct Tux64BootStage1Fsm *)
#define TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(identifier) \
   static void identifier (struct Tux64BootStage1Fsm * fsm)
#define TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(identifier) \
   static void identifier (struct Tux64BootStage1Fsm *)
#define TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(identifier) \
   static void identifier (struct Tux64BootStage1Fsm * fsm)

TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_delay);
TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_test);

TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_test);

static Tux64Boolean
tux64_boot_stage1_fsm_checksum_enable(void) {
   if (!TUX64_BOOT_CONFIG_CHECKSUM) {
      return TUX64_BOOLEAN_FALSE;
   }

   return (tux64_boot_stage1_boot_header_flag_no_checksum() == TUX64_BOOLEAN_FALSE);
}

static Tux64Boolean
tux64_boot_stage1_fsm_delay_enable(void) {
   if (!TUX64_BOOT_CONFIG_DELAY) {
      return TUX64_BOOLEAN_FALSE;
   }

   return (tux64_boot_stage1_boot_header_flag_no_delay() == TUX64_BOOLEAN_FALSE);
}

static void
tux64_boot_stage1_fsm_transition(
   struct Tux64BootStage1Fsm * fsm,
   Tux64BootStage1FsmPfnTransition transition
) {
   struct Tux64BootStage1FsmMemoryDelay * mem_delay;

   if (tux64_boot_stage1_fsm_delay_enable() == TUX64_BOOLEAN_FALSE) {
      transition(fsm);
      return;
   }

   mem_delay = &fsm->memory.delay;
   mem_delay->transition = transition;
   mem_delay->ticks_remaining = TUX64_LITERAL_UINT32(TUX64_BOOT_CONFIG_DELAY_TICKS);

   fsm->state = tux64_boot_stage1_fsm_state_delay;
   return;
}

TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(tux64_boot_stage1_fsm_state_delay) {
   struct Tux64BootStage1FsmMemoryDelay * mem;

   mem = &fsm->memory.delay;

   if (--mem->ticks_remaining != TUX64_LITERAL_UINT32(0u)) {
      return;
   }

   mem->transition(fsm);
   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_test) {
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
   const struct Tux64PlatformMipsN64BootHeaderFileKernel * kernel;
   const struct Tux64PlatformMipsN64BootHeaderFile * initramfs;
   Tux64UInt32 kernel_length;
   Tux64UInt32 kernel_memory;
   Tux64UInt32 initramfs_length;
   Tux64UInt32 kernel_addr_load;
   Tux64UInt32 kernel_addr_entry;

   if (tux64_boot_stage1_boot_header_flag_memory_display() == TUX64_BOOLEAN_FALSE) {
      return;
   }

   kernel      = tux64_boot_stage1_boot_header_file_kernel();
   initramfs   = tux64_boot_stage1_boot_header_file_initramfs();

   kernel_length     = kernel->image.file.length + TUX64_LITERAL_UINT32(1u);
   kernel_memory     = kernel->image.memory;
   initramfs_length  = initramfs->length + TUX64_LITERAL_UINT32(1u);
   kernel_addr_load  = kernel->addr_load;
   kernel_addr_entry = kernel->addr_entry;

   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_memory_total);
   tux64_boot_stage1_format_mib(label, memory_total);
   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_memory_free);
   tux64_boot_stage1_format_mib(label, memory_free);
   tux64_boot_stage1_fbcon_skip_line();

   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_kernel_image);
   tux64_boot_stage1_format_mib(label, kernel_length);
   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_kernel_memory);
   tux64_boot_stage1_format_mib(label, kernel_memory);
   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_initramfs_image);
   tux64_boot_stage1_format_mib(label, initramfs_length);
   tux64_boot_stage1_fbcon_skip_line();

   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_kernel_address_load);
   tux64_boot_stage1_format_address(label, kernel_addr_load);
   label = tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_kernel_address_entry);
   tux64_boot_stage1_format_address(label, kernel_addr_entry);
   tux64_boot_stage1_fbcon_skip_line();

   return;
}

static void
tux64_boot_stage1_fsm_initialize_checksum(void) {
   if (!tux64_boot_stage1_fsm_checksum_enable()) {
      (void)tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_no_checksum);
      return;
   }

   return;
}

void
tux64_boot_stage1_fsm_initialize(
   struct Tux64BootStage1Fsm * fsm
) {
   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_INITIAL);

   if (TUX64_BOOT_CONFIG_SPLASH) {
      tux64_boot_stage1_fsm_initialize_splash();
   }

   if (TUX64_BOOT_CONFIG_MEMORY_DISPLAY) {
      tux64_boot_stage1_fsm_initialize_memory_display(
         tux64_boot_stage1_memory_total(),
         tux64_boot_stage1_memory_free()
      );
   }

   if (TUX64_BOOT_CONFIG_CHECKSUM) {
      tux64_boot_stage1_fsm_initialize_checksum();
   }

   tux64_boot_stage1_fsm_transition(fsm, tux64_boot_stage1_fsm_transition_test);
   return;
}

void
tux64_boot_stage1_fsm_execute(
   struct Tux64BootStage1Fsm * fsm
) {
   fsm->state(fsm);
   return;
}

