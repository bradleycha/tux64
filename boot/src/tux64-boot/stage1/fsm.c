/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/fsm.c - Implementations for main finite state   */
/*    machine.                                                                */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/fsm.h"

#include <tux64/memory.h>
#include <tux64/endian.h>
#include <tux64/bitwise.h>
#include <tux64/checksum.h>
#include <tux64/platform/mips/n64/boot.h>
#include "tux64-boot/pi.h"
#include "tux64-boot/rsp.h"
#include "tux64-boot/cache.h"
#include "tux64-boot/load.h"
#include "tux64-boot/exec.h"
#include "tux64-boot/halt.h"
#include "tux64-boot/stage1/status.h"
#include "tux64-boot/stage1/memory.h"
#include "tux64-boot/stage1/preempt.h"
#include "tux64-boot/stage1/video.h"
#include "tux64-boot/stage1/interrupt.h"
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

   kernel_file       = tux64_boot_stage1_boot_header_file_kernel();
   kernel_address    = kernel_file->addr_load;
   kernel_bytes      = kernel_file->image.file.length;

   kernel_allocation = &load_info->allocations.required.kernel;
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
   const struct Tux64PlatformMipsN64BootHeaderFile * file
) {
   /* this has to be aligned for PI DMA. */
   return tux64_boot_stage1_fsm_allocate_optional(
      load_info,
      allocation,
      status_flag,
      file->length,
      TUX64_LITERAL_UINT32(8u)
   );
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_kernel_args(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info
) {
   /* this has to be allocated dynamically so it can live after the */
   /* bootloaders are unloaded.  we could theoretically denote a static */
   /* region of memory for kernel args, but that is ugly and could cause more */
   /* problems down the line.  let's just allocate dynamically. */
   return tux64_boot_stage1_fsm_allocate(
      load_info,
      &load_info->allocations.required.kernel_args,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_KERNEL_ARGS),
      TUX64_LITERAL_UINT32(sizeof(struct Tux64BootExecKernelArguments)),
      TUX64_LITERAL_UINT32(TUX64_ALIGNOF(struct Tux64BootExecKernelArguments))
   );
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_initramfs(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info
) {
   return tux64_boot_stage1_fsm_allocate_optional_file(
      load_info,
      &load_info->allocations.optional.initramfs,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_INITRAMFS),
      tux64_boot_stage1_boot_header_file_initramfs()
   );
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_command_line(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info
) {
   return tux64_boot_stage1_fsm_allocate_optional_file(
      load_info,
      &load_info->allocations.optional.command_line,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_COMMAND_LINE),
      tux64_boot_stage1_boot_header_file_command_line()
   );
}

static Tux64Boolean
tux64_boot_stage1_fsm_allocate_boot_files(
   struct Tux64BootStage1FsmGlobalsLoadInfo * load_info
) {
   if (tux64_boot_stage1_fsm_allocate_kernel(load_info) == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOLEAN_FALSE;
   }
   if (tux64_boot_stage1_fsm_allocate_kernel_args(load_info) == TUX64_BOOLEAN_FALSE) {
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
      mem->checksum_expected.uint = tux64_endian_convert_uint32(file->checksum, TUX64_ENDIAN_FORMAT_BIG);
      tux64_checksum_fletcher_64_32.initialize(&mem->checksum_context);
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

   kernel = tux64_boot_stage1_boot_header_file_kernel();

   tux64_boot_stage1_fsm_transition_load_file_optional(
      fsm,
      &kernel->image.file,
      fsm->globals.load_info.allocations.required.kernel.address,
      &tux64_boot_stage1_strings_file_kernel,
      tux64_boot_stage1_fsm_transition_load_file_initramfs,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_KERNEL)
   );
   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_load_file_initramfs) {
   const struct Tux64PlatformMipsN64BootHeaderFile * initramfs;

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_INITRAMFS);

   initramfs = tux64_boot_stage1_boot_header_file_initramfs();

   tux64_boot_stage1_fsm_transition_load_file_optional(
      fsm,
      initramfs,
      fsm->globals.load_info.allocations.optional.initramfs.address,
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

   command_line = tux64_boot_stage1_boot_header_file_command_line();

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
      fsm->globals.load_info.allocations.optional.command_line.address,
      &tux64_boot_stage1_strings_file_command_line,
      transition_next,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_COMMAND_LINE)
   );
   return;
}

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_load_file_stage2) {
   const struct Tux64PlatformMipsN64BootHeaderFile * stage2;

   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_LOAD_FILE_STAGE2);

   stage2 = tux64_boot_stage1_boot_header_file_bootloader_stage2();

   tux64_boot_stage1_fsm_transition_load_file(
      fsm,
      stage2,
      (Tux64UInt32)(Tux64UIntPtr)fsm->globals.stage2.dma_buffer,
      &tux64_boot_stage1_strings_file_bootloader_stage2,
      tux64_boot_stage1_fsm_transition_boot_stage2
   );
   return;
}

/* WARNING: this must be aligned to an 8-byte boundary for use with both */
/* PI DMA, which requires the RDRAM address to be aligned to 8 bytes. */
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
      tux64_checksum_fletcher_64_32.digest(
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
   const Tux64UInt8 * checksum_computed;

   if (tux64_boot_stage1_fsm_checksum_enable() == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOLEAN_TRUE;
   }
   
   checksum_computed = tux64_checksum_fletcher_64_32.finalize(&mem->checksum_context);

   return tux64_memory_compare_with_equal_lengths(
      checksum_computed,
      mem->checksum_expected.bytes,
      TUX64_LITERAL_UINT32(TUX64_CHECKSUM_FLETCHER_64_32_DIGEST_BYTES)
   );
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

TUX64_BOOT_STAGE1_FSM_TRANSITION_DEFINITION(tux64_boot_stage1_fsm_transition_boot_kernel) {
   (void)tux64_boot_stage1_fbcon_label_push(&tux64_boot_stage1_strings_boot_kernel);
   
   /* for the same reason as we have in tux64_boot_stage1_fsm_halt(), we need */
   /* to delay for 1 frame so that the message below displays. */
   if (TUX64_BOOT_CONFIG_DELAY) {
      tux64_boot_stage1_fsm_delay(
         fsm,
         tux64_boot_stage1_fsm_transition_boot_kernel_wait,
         TUX64_LITERAL_UINT32(1u)
      );
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
      tux64_boot_stage1_fsm_delay(
         fsm,
         tux64_boot_stage1_fsm_transition_boot_stage2_wait,
         TUX64_LITERAL_UINT32(1u)
      );
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
   const struct Tux64BootLoadAllocations * allocations;
   Tux64UInt32 addr_arguments;
   struct Tux64BootExecKernelArguments * arguments;
   Tux64UInt32 initramfs_address;
   Tux64UInt32 initramfs_bytes;
   Tux64UInt32 command_line_address;
   Tux64UInt32 total_memory;

   entrypoint = (const void *)tux64_boot_stage1_boot_header_file_kernel()->addr_entry;

   allocations = &fsm->globals.load_info.allocations;

   addr_arguments = fsm->globals.load_info.allocations.required.kernel_args.address;
   arguments      = (struct Tux64BootExecKernelArguments *)(Tux64UIntPtr)addr_arguments;

   initramfs_address    = allocations->optional.initramfs.address;
   initramfs_bytes      = tux64_boot_stage1_boot_header_file_initramfs()->length;
   command_line_address = allocations->optional.command_line.address;
   total_memory         = tux64_boot_stage1_memory_total();

   arguments->initramfs_address     = tux64_endian_convert_uint32(initramfs_address, TUX64_ENDIAN_FORMAT_BIG);
   arguments->initramfs_bytes       = tux64_endian_convert_uint32(initramfs_bytes, TUX64_ENDIAN_FORMAT_BIG);
   arguments->command_line_address  = tux64_endian_convert_uint32(command_line_address, TUX64_ENDIAN_FORMAT_BIG);
   arguments->total_memory          = tux64_endian_convert_uint32(total_memory, TUX64_ENDIAN_FORMAT_BIG);

   tux64_boot_stage1_fsm_reset_hardware();
   tux64_boot_exec_kernel(entrypoint, arguments);
   TUX64_UNREACHABLE;
}

TUX64_BOOT_STAGE1_FSM_STATE_DEFINITION(tux64_boot_stage1_fsm_state_boot_stage2) {
   struct Tux64BootRspDmaTransfer transfer;
   Tux64UInt16 stage2_bytes;

   stage2_bytes = (Tux64UInt16)tux64_boot_stage1_boot_header_file_bootloader_stage2()->length;

   /* we have to load stage-2 into RSP IMEM first.  we don't have to wait for */
   /* completion before the call because we're coming from the start of a new */
   /* frame.  thus, the RSP DMA engine isn't active.  note that we have to    */
   /* split this into two 'rows', as we allow a maximum of 4096 bytes.        */
   /* however, row_bytes_copy maxes out at 4095 bytes.  this does cause       */
   /* problems if stage-2 is an odd number of bytes.  we solve this by not    */
   /* producing stage-2 binaries of odd length, 4head.                        */
   transfer.addr_rsp_mem   = TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RSP_IMEM);
   transfer.addr_rdram     = (Tux64UInt32)(Tux64UIntPtr)fsm->globals.stage2.dma_buffer;
   transfer.row_bytes_copy = stage2_bytes / TUX64_LITERAL_UINT16(2u);
   transfer.row_bytes_skip = TUX64_LITERAL_UINT16(0u);
   transfer.row_count      = TUX64_LITERAL_UINT8(1u);
   tux64_boot_rsp_dma_start(&transfer, TUX64_BOOT_RSP_DMA_DESTINATION_RSP_MEMORY);

   /* TODO: DMA the boot header and allocations into RSP DMEM. */

   /* we now have to flush all DMA operations since we will now begin */
   /* executing stage-2. */
   tux64_boot_rsp_dma_wait_idle();

   tux64_boot_stage1_fsm_reset_hardware();
   tux64_boot_exec_stage2(
      tux64_boot_stage1_memory_total(),
      fsm->globals.load_info.status
   );
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
   tux64_boot_stage1_status_code_write(TUX64_BOOT_STAGE1_STATUS_CODE_MAIN_STATE_START);

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

