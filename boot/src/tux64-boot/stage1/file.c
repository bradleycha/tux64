/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/file.c - Implementations for file loader.       */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/file.h"

#include <tux64/memory.h>
#include <tux64/endian.h>
#include <tux64/checksum.h>
#include "tux64-boot/pi.h"
#include "tux64-boot/cache.h"
#include "tux64-boot/flag.h"

void
tux64_boot_stage1_file_load_initialize(
   struct Tux64BootStage1FileLoadContext * context,
   const struct Tux64PlatformMipsN64BootHeaderFile * file,
   Tux64UInt32 load_address
) {
   context->iter_addr_cart    = file->addr_cart;
   context->iter_addr_rdram   = load_address;
   context->bytes_remaining   = file->length;

   if (tux64_boot_flag_checksum() == TUX64_BOOLEAN_TRUE) {
      context->checksum_expected.uint = tux64_endian_convert_uint32(file->checksum, TUX64_ENDIAN_FORMAT_BIG);
      tux64_checksum_fletcher_64_32.initialize(&context->checksum_context);
   }

   return;
}

static enum Tux64BootStage1FileLoadPollStatus
tux64_boot_stage1_file_load_block(
   struct Tux64BootStage1FileLoadContext * context,
   Tux64UInt32 block_size
) {
   struct Tux64BootPiDmaTransfer pi_dma_transfer;
   enum Tux64BootPiDmaStatus pi_status;
   Tux64UInt32 addr_rdram;

   addr_rdram = context->iter_addr_rdram;

   pi_dma_transfer.addr_pibus = context->iter_addr_cart;
   pi_dma_transfer.addr_rdram = addr_rdram;
   pi_dma_transfer.bytes      = block_size - TUX64_LITERAL_UINT32(1u);
   tux64_boot_pi_dma_start(&pi_dma_transfer, TUX64_BOOT_PI_DMA_DESTINATION_RDRAM);

   context->iter_addr_cart  += block_size;
   context->iter_addr_rdram += block_size;
   context->bytes_remaining -= block_size;

   pi_status = tux64_boot_pi_dma_wait_idle();

   /* invalidate both instruction and data cache since we may be loading */
   /* executables, which have both code and data.  make sure to invalidate */
   /* after the DMA transfer to prevent unrelated caches to load garbage. */
   tux64_boot_cache_invalidate(
      (const void *)(Tux64UIntPtr)addr_rdram,
      block_size
   );

   if (pi_status == TUX64_BOOT_PI_DMA_STATUS_IO_ERROR) {
      return TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_IO_ERROR;
   }

   if (tux64_boot_flag_checksum() == TUX64_BOOLEAN_TRUE) {
      tux64_checksum_fletcher_64_32.digest(
         &context->checksum_context,
         (const Tux64UInt8 *)(Tux64UIntPtr)addr_rdram,
         block_size
      );
   }

   return TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_COMPLETE;
}

static enum Tux64BootStage1FileLoadPollStatus
tux64_boot_stage1_file_load_finalize(
   struct Tux64BootStage1FileLoadContext * context
) {
   const Tux64UInt8 * checksum_computed;

   if (tux64_boot_flag_checksum() == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_COMPLETE;
   }

   checksum_computed = tux64_checksum_fletcher_64_32.finalize(&context->checksum_context);

   if (tux64_memory_compare_with_equal_lengths(
      checksum_computed,
      context->checksum_expected.bytes,
      TUX64_LITERAL_UINT32(TUX64_CHECKSUM_FLETCHER_64_32_DIGEST_BYTES)
   ) == TUX64_BOOLEAN_FALSE) {
      return TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_BAD_CHECKSUM;
   }

   return TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_COMPLETE;
}

#define TUX64_BOOT_STAGE1_FILE_LOAD_BLOCK_SIZE\
   (4u * 1024u) /* 4KiB */

struct Tux64BootStage1FileLoadPollResult
tux64_boot_stage1_file_load_poll(
   struct Tux64BootStage1FileLoadContext * context
) {
   struct Tux64BootStage1FileLoadPollResult result;
   Tux64UInt32 block_size;

   if (context->bytes_remaining >= TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_FILE_LOAD_BLOCK_SIZE)) {
      block_size = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_FILE_LOAD_BLOCK_SIZE);
   } else {
      block_size = context->bytes_remaining;
   }

   result.bytes = block_size;

   result.status = tux64_boot_stage1_file_load_block(context, block_size);
   if (result.status != TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_COMPLETE) {
      return result;
   }

   if (context->bytes_remaining != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_BUSY;
      return result;
   }

   result.status = tux64_boot_stage1_file_load_finalize(context);
   return result;
}

