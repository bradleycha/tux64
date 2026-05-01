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
#include "tux64-boot/pi.h"
#include "tux64-boot/checksum.h"
#include "tux64-boot/cache.h"
#include "tux64-boot/halt.h"
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
TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_halt);
TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_load_file);
TUX64_BOOT_STAGE1_FSM_STATE_DECLARATION(tux64_boot_stage1_fsm_state_test);

TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_halt);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_start);
TUX64_BOOT_STAGE1_FSM_TRANSITION_DECLARATION(tux64_boot_stage1_fsm_transition_load_file_kernel);
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
tux64_boot_stage1_fsm_transition(
   struct Tux64BootStage1Fsm * fsm,
   Tux64BootStage1FsmPfnTransition transition
) {

   if (tux64_boot_stage1_fsm_delay_enable() == TUX64_BOOLEAN_FALSE) {
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
      tux64_boot_stage1_fsm_delay(
         fsm,
         tux64_boot_stage1_fsm_transition_halt,
         TUX64_LITERAL_UINT32(1u)
      );
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
tux64_boot_stage1_fsm_enough_memory_to_boot(void) {
   const struct Tux64PlatformMipsN64BootHeaderFileKernel * kernel;
   const struct Tux64PlatformMipsN64BootHeaderFile * initramfs;
   const struct Tux64PlatformMipsN64BootHeaderFile * command_line;
   Tux64UInt32 required_memory;

   /* this is just a basic check to see if we have enough total system memory */
   /* to boot, regardless of stage-2. */

   kernel         = tux64_boot_stage1_boot_header_file_kernel();
   initramfs      = tux64_boot_stage1_boot_header_file_initramfs();
   command_line   = tux64_boot_stage1_boot_header_file_command_line();

   required_memory = kernel->image.memory
      + initramfs->length
      + command_line->length;

   if (required_memory > tux64_boot_stage1_memory_total()) {
      return TUX64_BOOLEAN_FALSE;
   }

   return TUX64_BOOLEAN_TRUE;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_start) {
   if (tux64_boot_stage1_fsm_enough_memory_to_boot() == TUX64_BOOLEAN_FALSE) {
      tux64_boot_stage1_fsm_halt(fsm, &tux64_boot_stage1_strings_error_no_memory);
      return;
   }

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

   mem->transition_next = transition_next;
   mem->iter_addr_rdram = load_address;
   mem->iter_addr_cart  = file->addr_cart;
   mem->bytes_remaining = file->length;

   if (tux64_boot_stage1_fsm_checksum_enable() == TUX64_BOOLEAN_TRUE) {
      mem->checksum_expected = file->checksum;
      tux64_boot_checksum_fletcher_64_32_initialize(&mem->checksum_context);
   }

   tux64_boot_stage1_format_percentage_initialize(
      &mem->percentage_progress,
      mem->bytes_remaining
   );

   label_characters = name->length
      + TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_LOADING)
      + TUX64_LITERAL_UINT8(TUX64_BOOT_STAGE1_FORMAT_BUFFER_CHARACTERS_PERCENTAGE);

   label = tux64_boot_stage1_fbcon_label_allocate(label_characters);
   tux64_boot_stage1_format_loading(label, name);
   tux64_boot_stage1_format_percentage(&mem->percentage_progress, label);
   mem->label_percentage_progress = label;

   fsm->state = tux64_boot_stage1_fsm_state_load_file;
   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_load_file_kernel) {
   const struct Tux64PlatformMipsN64BootHeaderFileKernel * kernel;

   kernel = tux64_boot_stage1_boot_header_file_kernel();

   /* TODO: transition to loading initramfs and command-line (or deferring to */
   /* stage-2 if we are fighting for memory) instead of the test state.  also */
   /* create a contingency to defer to stage-2 if we can't allocate the */
   /* memory with stage-1 loaded. */

   tux64_boot_stage1_fsm_transition_load_file(
      fsm,
      &kernel->image.file,
      kernel->addr_load,
      &tux64_boot_stage1_strings_file_kernel,
      tux64_boot_stage1_fsm_transition_test
   );
   return;
}

/* WARNING: this must be aligned to an 8-byte boundary for use with both */
/* PI DMA, which requires the RDRAM address to be aligned to 8 bytes, and */
/* our specialized checksum function which requires both alignment to 4 bytes */
/* as well as byte lengths which are a multiple of 4. */
#define TUX64_BOOT_STAGE1_FSM_LOAD_FILE_BLOCK_SIZE \
   (4u * 1024u) /* 4KiB */

static enum Tux64BootPiDmaStatus
tux64_boot_stage1_fsm_load_file_block(
   struct Tux64BootStage1FsmMemoryLoadFile * mem,
   const struct Tux64BootPiDmaTransfer * transfer
) {
   enum Tux64BootPiDmaStatus status;
   Tux64UInt32 bytes;

   tux64_boot_pi_dma_start(transfer, TUX64_BOOT_PI_DMA_DESTINATION_RDRAM);

   bytes = transfer->bytes + TUX64_LITERAL_UINT32(1u);

   /* we always invalidate both instruction and data cache because the file */
   /* could contain executable code, but we don't really care to set up a */
   /* conditional to check for such files.  since we invalidate cache during */
   /* the DMA transfer, we should have more than enough time to invalidate */
   /* both. */
   tux64_boot_cache_invalidate(
      (const void *)(Tux64UIntPtr)transfer->addr_rdram,
      bytes
   );

   status = tux64_boot_pi_dma_wait_idle();
   if (status == TUX64_BOOT_PI_DMA_STATUS_IO_ERROR) {
      return status;
   }

   if (tux64_boot_stage1_fsm_checksum_enable() == TUX64_BOOLEAN_TRUE) {
      /* since we are loading via PI DMA, which requires 8-byte aligned */
      /* addresses, and our files are all aligned to 4 bytes in size, we can */
      /* safely call this as-is. */
      tux64_boot_checksum_fletcher_64_32_digest(
         &mem->checksum_context,
         (const Tux64UInt8 *)transfer->addr_rdram,
         bytes
      );
   }

   return status;
}

static Tux64Boolean
tux64_boot_stage1_fsm_verify_file_checksum(
   struct Tux64BootStage1FsmMemoryLoadFile * mem
) {
   Tux64UInt32 checksum_computed;

   if (tux64_boot_stage1_fsm_checksum_enable() == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOLEAN_TRUE;
   }
   
   checksum_computed = tux64_boot_checksum_fletcher_64_32_finalize(&mem->checksum_context);

   return (checksum_computed == mem->checksum_expected) ?
      TUX64_BOOLEAN_TRUE :
      TUX64_BOOLEAN_FALSE;
}

TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(tux64_boot_stage1_fsm_state_load_file) {
   struct Tux64BootStage1FsmMemoryLoadFile * mem;
   struct Tux64BootPiDmaTransfer transfer;
   enum Tux64BootPiDmaStatus pi_status;
   Tux64UInt32 bytes_transferred;
   Tux64Boolean dma_complete;
   Tux64Boolean valid_checksum;

   mem = &fsm->memory.load_file;

   transfer.addr_pibus  = mem->iter_addr_cart;
   transfer.addr_rdram  = mem->iter_addr_rdram;
   transfer.bytes       = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_FSM_LOAD_FILE_BLOCK_SIZE - 1u);

   bytes_transferred = TUX64_LITERAL_UINT32(0u);
   dma_complete = TUX64_BOOLEAN_FALSE;

   do {
      if (mem->bytes_remaining >= TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_FSM_LOAD_FILE_BLOCK_SIZE)) {
         mem->bytes_remaining -= TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_FSM_LOAD_FILE_BLOCK_SIZE);
         bytes_transferred    += TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_FSM_LOAD_FILE_BLOCK_SIZE);
      } else {
         transfer.bytes = mem->bytes_remaining - TUX64_LITERAL_UINT32(1u);
         bytes_transferred += mem->bytes_remaining;
         dma_complete = TUX64_BOOLEAN_TRUE;
      }

      pi_status = tux64_boot_stage1_fsm_load_file_block(mem, &transfer);
      if (pi_status == TUX64_BOOT_PI_DMA_STATUS_IO_ERROR) {
         break;
      }
      if (dma_complete == TUX64_BOOLEAN_TRUE) {
         break;
      }

      transfer.addr_pibus  += TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_FSM_LOAD_FILE_BLOCK_SIZE);
      transfer.addr_rdram  += TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_FSM_LOAD_FILE_BLOCK_SIZE);
   } while (tux64_boot_stage1_preempt_yield() == TUX64_BOOLEAN_FALSE);

   tux64_boot_stage1_format_percentage_accumulate(
      &mem->percentage_progress,
      bytes_transferred
   );
   tux64_boot_stage1_format_percentage(
      &mem->percentage_progress,
      mem->label_percentage_progress
   );

   if (pi_status == TUX64_BOOT_PI_DMA_STATUS_IO_ERROR) {
      tux64_boot_stage1_fsm_halt(fsm, &tux64_boot_stage1_strings_error_io);
      return;
   }

   if (dma_complete == TUX64_BOOLEAN_TRUE) {
      valid_checksum = tux64_boot_stage1_fsm_verify_file_checksum(mem);
      if (valid_checksum == TUX64_BOOLEAN_FALSE) {
         tux64_boot_stage1_fsm_halt(fsm, &tux64_boot_stage1_strings_error_checksum);
         return;
      }
      
      tux64_boot_stage1_fsm_transition(fsm, mem->transition_next);
      return;
   }

   mem->iter_addr_cart  = transfer.addr_pibus;
   mem->iter_addr_rdram = transfer.addr_rdram;
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

   if (mem->percentage.progress == mem->percentage.maximum) {
      tux64_boot_stage1_fsm_halt(fsm, &tux64_boot_stage1_strings_error_test);
   }

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

