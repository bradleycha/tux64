/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/fsm.c - Implementations for main finite state   */
/*    machine.                                                                */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/fsm.h"

#include <tux64/bitwise.h>
#include <tux64/platform/mips/n64/boot.h>
#include "tux64-boot/rsp.h"
#include "tux64-boot/load.h"
#include "tux64-boot/exec.h"
#include "tux64-boot/halt.h"
#include "tux64-boot/header.h"
#include "tux64-boot/layout.h"
#include "tux64-boot/load.h"
#include "tux64-boot/flag.h"
#include "tux64-boot/stage1/status.h"
#include "tux64-boot/stage1/memory.h"
#include "tux64-boot/stage1/preempt.h"
#include "tux64-boot/stage1/video.h"
#include "tux64-boot/stage1/interrupt.h"
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
TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_halt);
TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_load_file);
TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_boot_kernel);
TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_boot_stage2);

TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_halt);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_start);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_load_file_kernel);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_load_file_initramfs);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_load_file_command_line);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_load_file_stage2);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_boot_kernel);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_boot_kernel_wait);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_boot_stage2);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_boot_stage2_wait);

/* defined by the linker in stage1.ld.S, allows us to load stage-2 directly. */
__attribute__((section(".file_stage2")))
extern Tux64UInt8
tux64_boot_stage1_file_stage2[TUX64_BOOT_LAYOUT_STAGE2_LOAD_BYTES_MAXIMUM];

static void
tux64_boot_stage1_fsm_delay(
   struct Tux64BootStage1Fsm * fsm,
   Tux64BootStage1FsmPfnTransition transition,
   Tux64UInt32 delay_ticks
) {
   struct Tux64BootStage1FsmMemoryDelay * mem_delay;

   mem_delay = &fsm->memory.delay;

   mem_delay->transition      = transition;
   mem_delay->ticks_remaining = delay_ticks;

   fsm->state = tux64_boot_stage1_fsm_state_delay;
   return;
}

static void
tux64_boot_stage1_fsm_delay_one(
   struct Tux64BootStage1Fsm * fsm,
   Tux64BootStage1FsmPfnTransition transition
) {
   tux64_boot_stage1_fsm_delay(fsm, transition, TUX64_LITERAL_UINT32(1u));
   return;
}

static void
tux64_boot_stage1_fsm_transition(
   struct Tux64BootStage1Fsm * fsm,
   Tux64BootStage1FsmPfnTransition transition
) {
   if (tux64_boot_flag_delay() == TUX64_BOOLEAN_FALSE) {
      transition(fsm);
      return;
   }

   tux64_boot_stage1_fsm_delay(
      fsm,
      transition,
      TUX64_LITERAL_UINT32(TUX64_BOOT_CONFIG_DELAY_TICKS)
   );
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

static void
tux64_boot_stage1_fsm_halt(
   struct Tux64BootStage1Fsm * fsm,
   const struct Tux64BootStage1FbconText * reason
) {
   Tux64BootStage1FbconLabel label;
   Tux64UInt8 characters;

   characters = TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_ERROR) + reason->length;
   label = tux64_boot_stage1_fbcon_label_allocate(characters);
   tux64_boot_stage1_format_error(label, reason);

   /* we try to delay for one tick, even if --no-delay is passed, so we have */
   /* time to render the frame before halting the CPU.  we only respect no */
   /* delay when it's compiled out because at that point, it's clear the */
   /* user knows what they're doing. */
   if (TUX64_BOOT_CONFIG_DELAY) {
      tux64_boot_stage1_fsm_delay_one(fsm, tux64_boot_stage1_fsm_transition_halt);
   } else {
      /* this works for all but the initial state because we are already in a */
      /* state, so the halt state only gets executed on the next tick. */
      fsm->state = tux64_boot_stage1_fsm_state_halt;
   }

   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_halt) {
   fsm->state = tux64_boot_stage1_fsm_state_halt;
   return;
}

TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(tux64_boot_stage1_fsm_state_halt) {
   /* halting is its own dedicated state so that we have time to render error */
   /* messages or other diagnostic information before halting the CPU.  this  */
   /* is preferable to directly calling tux64_boot_halt() wherever possible.  */
   (void)fsm;
   tux64_boot_halt();
   TUX64_UNREACHABLE;
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_kernel(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info
) {
   const struct Tux64PlatformMipsN64BootHeaderFileKernel * kernel_file;
   Tux64UInt32 kernel_address;
   Tux64UInt32 kernel_bytes;
   struct Tux64BootLoadAllocationsFile * kernel_allocation;
   Tux64Boolean result;

   /* the kernel is a special case because it's the only boot file which has */
   /* to be loaded at a specific address.  the rest of the boot files can be */
   /* loaded anywhere. */

   kernel_file       = tux64_boot_header_file_kernel();
   kernel_address    = kernel_file->addr_load;
   kernel_bytes      = kernel_file->image.file.length;

   kernel_allocation = &tux64_boot_load_allocations.required.kernel;
   result            = TUX64_BOOLEAN_FALSE;

   if (tux64_boot_stage1_memory_stage1_alloc_inplace(kernel_address, kernel_bytes) == TUX64_BOOLEAN_TRUE) {
      result = TUX64_BOOLEAN_TRUE;

      load_info->status = tux64_bitwise_flags_set_uint8(
         load_info->status,
         TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_KERNEL)
      );
   }

   if (tux64_boot_stage1_memory_stage2_alloc_inplace(kernel_address, kernel_bytes) == TUX64_BOOLEAN_TRUE) {
      result = TUX64_BOOLEAN_TRUE;
   }

   kernel_allocation->address = kernel_address;
   return result;
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info,
   struct Tux64BootLoadAllocationsFile * allocation,
   Tux64BootLoadStatus status_flag,
   Tux64UInt32 bytes,
   Tux64UInt32 alignment
) {
   Tux64UInt32 address_stage1;
   Tux64UInt32 address_stage2;

   /* TODO: this will break if we load stage1 -> stage2 -> stage1 due to the */
   /* heaps desynchronizing. */
   address_stage1 = tux64_boot_stage1_memory_stage1_alloc(bytes, alignment);
   address_stage2 = tux64_boot_stage1_memory_stage2_alloc(bytes, alignment);

   if (address_stage1 != TUX64_LITERAL_UINT32(0u)) {
      load_info->status = tux64_bitwise_flags_set_uint8(load_info->status, status_flag);
      allocation->address = address_stage1;
      return TUX64_BOOLEAN_TRUE;
   }
   if (address_stage2 != TUX64_LITERAL_UINT32(0u)) {
      allocation->address = address_stage2;
      return TUX64_BOOLEAN_TRUE;
   }

   return TUX64_BOOLEAN_FALSE;
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_optional(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info,
   struct Tux64BootLoadAllocationsFile * allocation,
   Tux64BootLoadStatus status_flag,
   Tux64UInt32 bytes,
   Tux64UInt32 alignment
) {
   if (bytes == TUX64_LITERAL_UINT32(0u)) {
      load_info->status = tux64_bitwise_flags_set_uint8(load_info->status, status_flag);
      allocation->address = TUX64_LITERAL_UINT32(0u);
      return TUX64_BOOLEAN_TRUE;
   }

   return tux64_boot_stage1_fsm_allocate(
      load_info,
      allocation,
      status_flag,
      bytes,
      alignment
   );
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_optional_file(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info,
   struct Tux64BootLoadAllocationsFile * allocation,
   Tux64BootLoadStatus status_flag,
   const struct Tux64PlatformMipsN64BootHeaderFile * file,
   Tux64UInt32 alignment
) {
   return tux64_boot_stage1_fsm_allocate_optional(
      load_info,
      allocation,
      status_flag,
      file->length,
      alignment
   );
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_initramfs(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info
) {
   /* the kernel expects the initrd to be page-aligned.  we assume a default */
   /* page size of 4096 bytes and align to that.  if the kernel has a */
   /* different page size, it can relocate the initrd itself. */
   return tux64_boot_stage1_fsm_allocate_optional_file(
      load_info,
      &tux64_boot_load_allocations.optional.initramfs,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_INITRAMFS),
      tux64_boot_header_file_initramfs(),
      TUX64_LITERAL_UINT32(4096u)
   );
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_command_line(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info
) {
   /* this has to be aligned for PI DMA. */
   return tux64_boot_stage1_fsm_allocate_optional_file(
      load_info,
      &tux64_boot_load_allocations.optional.command_line,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_COMMAND_LINE),
      tux64_boot_header_file_command_line(),
      TUX64_LITERAL_UINT32(8u)
   );
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_boot_files(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info
) {
   if (tux64_boot_stage1_fsm_allocate_kernel(load_info) == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOLEAN_FALSE;
   }
   if (tux64_boot_stage1_fsm_allocate_initramfs(load_info) == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOLEAN_FALSE;
   }
   if (tux64_boot_stage1_fsm_allocate_command_line(load_info) == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOLEAN_FALSE;
   }

   return TUX64_BOOLEAN_TRUE;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_start) {
   if (tux64_boot_stage1_fsm_allocate_boot_files(&fsm->globals.load_info) == TUX64_BOOLEAN_FALSE) {
      tux64_boot_stage1_fsm_halt(fsm, &tux64_boot_stage1_strings_error_no_memory);
      return;
   }

   /* we set this early so, in the event of loading stage-2, this is already */
   /* initialized.  thus, we don't have to include this code in stage-2. */
   tux64_boot_exec_kernel_arguments_initialize(
      tux64_boot_load_allocations.optional.initramfs.address,
      tux64_boot_header_file_initramfs()->length,
      tux64_boot_load_allocations.optional.command_line.address,
      tux64_boot_stage1_memory_total()
   );

   /* directly called to avoid unnecessary extra delay */
   tux64_boot_stage1_fsm_transition_load_file_kernel(fsm);
   return;
}

static void
tux64_boot_stage1_fsm_transition_load_file(
   struct Tux64BootStage1Fsm * fsm,
   const struct Tux64PlatformMipsN64BootHeaderFile * file,
   Tux64UInt32 load_address,
   const struct Tux64BootStage1FbconText * name,
   Tux64BootStage1FsmPfnTransition transition_next
) {
   struct Tux64BootStage1FsmMemoryLoadFile * mem;
   Tux64BootStage1FbconLabel label;
   Tux64UInt8 label_characters;

   mem = &fsm->memory.load_file;

   tux64_boot_stage1_file_load_initialize(
      &mem->file_load_context,
      file,
      load_address
   );

   tux64_boot_stage1_format_percentage_initialize(
      &mem->percentage_progress,
      file->length
   );

   label_characters = name->length
      + TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_LOADING)
      + TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_PERCENTAGE);

   label = tux64_boot_stage1_fbcon_label_allocate(label_characters);
   tux64_boot_stage1_format_loading(label, name);
   tux64_boot_stage1_format_percentage(&mem->percentage_progress, label);
   mem->label_percentage_progress = label;

   mem->transition_next = transition_next;

   fsm->state = tux64_boot_stage1_fsm_state_load_file;
   return;
}

static void
tux64_boot_stage1_fsm_transition_load_file_optional(
   struct Tux64BootStage1Fsm * fsm,
   const struct Tux64PlatformMipsN64BootHeaderFile * file,
   Tux64UInt32 load_address,
   const struct Tux64BootStage1FbconText * name,
   Tux64BootStage1FsmPfnTransition transition_next,
   Tux64BootLoadStatus status_flag
) {
   if (
      load_address == TUX64_LITERAL_UINT32(0u) ||
      tux64_bitwise_flags_check_one_uint8(fsm->globals.load_info.status, status_flag) == TUX64_BOOLEAN_FALSE
   ) {
      /* call directly since we're already coming from a transition, so we */
      /* don't delay twice. */
      transition_next(fsm);
      return;
   }

   tux64_boot_stage1_fsm_transition_load_file(
      fsm,
      file,
      load_address,
      name,
      transition_next
   );
   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_load_file_kernel) {
   const struct Tux64PlatformMipsN64BootHeaderFileKernel * kernel;

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_KERNEL);

   kernel = tux64_boot_header_file_kernel();

   tux64_boot_stage1_fsm_transition_load_file_optional(
      fsm,
      &kernel->image.file,
      tux64_boot_load_allocations.required.kernel.address,
      &tux64_boot_stage1_strings_file_kernel,
      tux64_boot_stage1_fsm_transition_load_file_initramfs,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_KERNEL)
   );
   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_load_file_initramfs) {
   const struct Tux64PlatformMipsN64BootHeaderFile * initramfs;

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_INITRAMFS);

   initramfs = tux64_boot_header_file_initramfs();

   tux64_boot_stage1_fsm_transition_load_file_optional(
      fsm,
      initramfs,
      tux64_boot_load_allocations.optional.initramfs.address,
      &tux64_boot_stage1_strings_file_initramfs,
      tux64_boot_stage1_fsm_transition_load_file_command_line,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_INITRAMFS)
   );
   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_load_file_command_line) {
   const struct Tux64PlatformMipsN64BootHeaderFile * command_line;
   Tux64BootStage1FsmPfnTransition transition_next;

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_COMMAND_LINE);

   command_line = tux64_boot_header_file_command_line();

   /* if we have everything loadable from stage-1, we can boot the kernel */
   /* directly.  otherwise, we need the stage-2 loader. */
   if (fsm->globals.load_info.status == TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_CONCURRENT_WITH_STAGE1)) {
      transition_next = tux64_boot_stage1_fsm_transition_boot_kernel;
   } else {
      transition_next = tux64_boot_stage1_fsm_transition_load_file_stage2;
   }

   tux64_boot_stage1_fsm_transition_load_file_optional(
      fsm,
      command_line,
      tux64_boot_load_allocations.optional.command_line.address,
      &tux64_boot_stage1_strings_file_command_line,
      transition_next,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_COMMAND_LINE)
   );
   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_load_file_stage2) {
   const struct Tux64PlatformMipsN64BootHeaderFile * stage2;

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_STAGE2);

   stage2 = tux64_boot_header_file_bootloader_stage2();

   if (
      TUX64_BOOT_CONFIG_DEBUG &&
      stage2->length > TUX64_LITERAL_UINT32(TUX64_BOOT_LAYOUT_STAGE2_LOAD_BYTES_MAXIMUM)
   ) {
      tux64_boot_halt();
      TUX64_UNREACHABLE;
   }

   tux64_boot_stage1_fsm_transition_load_file(
      fsm,
      stage2,
      (Tux64UInt32)(Tux64UIntPtr)tux64_boot_stage1_file_stage2,
      &tux64_boot_stage1_strings_file_bootloader_stage2,
      tux64_boot_stage1_fsm_transition_boot_stage2
   );
   return;
}

TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(tux64_boot_stage1_fsm_state_load_file) {
   struct Tux64BootStage1FsmMemoryLoadFile * mem;
   struct Tux64BootStage1FileLoadPollResult poll_result;

   mem = &fsm->memory.load_file;

   do {
      poll_result = tux64_boot_stage1_file_load_poll(&mem->file_load_context);
      tux64_boot_stage1_format_percentage_accumulate(
         &mem->percentage_progress,
         poll_result.bytes
      );

      if (poll_result.status != TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_BUSY) {
         break;
      }
   } while (tux64_boot_stage1_preempt_yield() == TUX64_BOOLEAN_FALSE);
   
   tux64_boot_stage1_format_percentage(
      &mem->percentage_progress,
      mem->label_percentage_progress
   );

   switch (poll_result.status) {
      case TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_BUSY:
         break;

      case TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_COMPLETE:
         tux64_boot_stage1_fsm_transition(fsm, mem->transition_next);
         return;
      
      case TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_IO_ERROR:
         tux64_boot_stage1_fsm_halt(fsm, &tux64_boot_stage1_strings_error_io);
         return;

      case TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_BAD_CHECKSUM:
         tux64_boot_stage1_fsm_halt(fsm, &tux64_boot_stage1_strings_error_checksum);
         return;

      default:
         TUX64_UNREACHABLE;
   }

   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_boot_kernel) {
   (void)tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_boot_kernel);
   
   /* for the same reason as we have in tux64_boot_stage1_fsm_halt(), we need */
   /* to delay for 1 frame so that the message below displays. */
   if (TUX64_BOOT_CONFIG_DELAY) {
      tux64_boot_stage1_fsm_delay_one(fsm, tux64_boot_stage1_fsm_transition_boot_kernel_wait);
   } else {
      /* execute directly to eliminate any delay. */
      tux64_boot_stage1_fsm_transition_boot_kernel_wait(fsm);
   }

   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_boot_kernel_wait) {
   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START_KERNEL);

   fsm->state = tux64_boot_stage1_fsm_state_boot_kernel;
   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_boot_stage2) {
   (void)tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_boot_stage2);

   /* same comments as above. */
   if (TUX64_BOOT_CONFIG_DELAY) {
      tux64_boot_stage1_fsm_delay_one(fsm, tux64_boot_stage1_fsm_transition_boot_stage2_wait);
   } else {
      tux64_boot_stage1_fsm_transition_boot_stage2_wait(fsm);
   }

   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_boot_stage2_wait) {
   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START_STAGE2);

   fsm->state = tux64_boot_stage1_fsm_state_boot_stage2;
   return;
}

static void
tux64_boot_stage1_fsm_reset_hardware(void) {
   /* since we're coming from either the start of a new frame or after */
   /* loading a file, no DMA engines should be active.  thus, all we need to */
   /* do is disable video output and disable interrupts. */
   tux64_boot_stage1_video_display_output(TUX64_BOOLEAN_FALSE);
   tux64_boot_stage1_interrupt_disable();
   return;
}

TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(tux64_boot_stage1_fsm_state_boot_kernel) {
   const void * entrypoint;

   entrypoint = (const void *)tux64_boot_header_file_kernel()->addr_entry;

   tux64_boot_stage1_fsm_reset_hardware();

   /* kernel arguments were initialized when we created our allocations. */
   tux64_boot_exec_kernel(entrypoint);

   (void)fsm;
   TUX64_UNREACHABLE;
}

TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(tux64_boot_stage1_fsm_state_boot_stage2) {
   tux64_boot_stage1_fsm_reset_hardware();
   tux64_boot_exec_stage2(fsm->globals.load_info.status);
   TUX64_UNREACHABLE;
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

   kernel      = tux64_boot_header_file_kernel();
   initramfs   = tux64_boot_header_file_initramfs();

   kernel_length     = kernel->image.file.length;
   kernel_memory     = kernel->image.memory;
   initramfs_length  = initramfs->length;
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
   (void)tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_no_checksum);
   return;
}

void
tux64_boot_stage1_fsm_initialize(
   struct Tux64BootStage1Fsm * fsm
) {
   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START);

   if (TUX64_BOOT_CONFIG_SPLASH) {
      tux64_boot_stage1_fsm_initialize_splash();
   }

   if (tux64_boot_flag_memory_display() == TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_fsm_initialize_memory_display(
         tux64_boot_stage1_memory_total(),
         tux64_boot_stage1_memory_free()
      );
   }

   if (tux64_boot_flag_checksum() == TUX64_BOOLEAN_TRUE) {
      tux64_boot_stage1_fsm_initialize_checksum();
   }

   tux64_boot_stage1_fsm_transition(fsm, tux64_boot_stage1_fsm_transition_start);
   return;
}

void
tux64_boot_stage1_fsm_execute(
   struct Tux64BootStage1Fsm * fsm
) {
   fsm->state(fsm);
   return;
}

