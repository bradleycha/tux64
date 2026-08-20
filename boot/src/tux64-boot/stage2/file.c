/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/file.c - File loading implementations.          */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage2/file.h"

#include <tux64/bitwise.h>
#include <tux64/checksum.h>
#include <tux64/memory.h>
#include <tux64/endian.h>
#include <tux64/platform/mips/n64/boot.h>
#include "tux64-boot/header.h"
#include "tux64-boot/halt.h"
#include "tux64-boot/load.h"
#include "tux64-boot/pi.h"
#include "tux64-boot/stage2/status.h"
#include "tux64-boot/stage2/allocations.h"

static void
tux64_boot_stage2_file_verify_checksum(
   const Tux64UInt8 * data,
   Tux64UInt32 bytes,
   Tux64UInt32 checksum
) {
   struct Tux64ChecksumContext ctx;
   union Tux64Int32 checksum_expected;
   const Tux64UInt8 * checksum_computed;

   checksum_expected.uint = tux64_endian_convert_uint32(checksum, TUX64_ENDIAN_FORMAT_BIG);

   tux64_checksum_fletcher_64_32.initialize(&ctx);
   tux64_checksum_fletcher_64_32.digest(&ctx, data, bytes);
   checksum_computed = tux64_checksum_fletcher_64_32.finalize(&ctx);

   if (tux64_memory_compare_with_equal_lengths(
      checksum_computed,
      checksum_expected.bytes,
      TUX64_LITERAL_UINT32(TUX64_CHECKSUM_FLETCHER_64_32_DIGEST_BYTES)
   ) == TUX64_BOOLEAN_FALSE) {
      tux64_boot_halt();
   }

   return;
}

static void
tux64_boot_stage2_file_load(
   Tux64UInt32 load_address,
   const struct Tux64PlatformMipsN64BootHeaderFile * file
) {
   struct Tux64BootPiDmaTransfer transfer;

   /* since stage-2 is non-interactive, file loading here is ez-pz. */
   transfer.addr_pibus  = file->addr_cart;
   transfer.addr_rdram  = load_address;
   transfer.bytes       = file->length - TUX64_LITERAL_UINT32(1u);
   tux64_boot_pi_dma_start(&transfer, TUX64_BOOT_PI_DMA_DESTINATION_RDRAM);
   tux64_boot_pi_dma_wait_idle();

   if (tux64_boot_header_flag_no_checksum() == TUX64_BOOLEAN_FALSE) {
      tux64_boot_stage2_file_verify_checksum(
         (const Tux64UInt8 *)load_address,
         file->length,
         file->checksum
      );
   }

   return;
}

static void
tux64_boot_stage2_file_load_optional(
   Tux64UInt32 load_address,
   const struct Tux64PlatformMipsN64BootHeaderFile * file
) {
   if (file->length == TUX64_LITERAL_UINT32(0u)) {
      return;
   }

   tux64_boot_stage2_file_load(load_address, file);
   return;
}

static void
tux64_boot_stage2_file_load_kernel(void) {
   tux64_boot_stage2_status_code_write(TUX64_BOOT_STAGE2_STATUS_CODE_LOAD_FILE_KERNEL);

   tux64_boot_stage2_file_load(
      tux64_boot_stage2_allocations.required.kernel.address,
      &tux64_boot_header_file_kernel()->image.file
   );

   return;
}

static void
tux64_boot_stage2_file_load_initramfs(void) {
   tux64_boot_stage2_status_code_write(TUX64_BOOT_STAGE2_STATUS_CODE_LOAD_FILE_INITRAMFS);

   tux64_boot_stage2_file_load_optional(
      tux64_boot_stage2_allocations.optional.initramfs.address,
      tux64_boot_header_file_initramfs()
   );

   return;
}

static void
tux64_boot_stage2_file_load_command_line(void) {
   tux64_boot_stage2_status_code_write(TUX64_BOOT_STAGE2_STATUS_CODE_LOAD_FILE_COMMAND_LINE);

   tux64_boot_stage2_file_load_optional(
      tux64_boot_stage2_allocations.optional.command_line.address,
      tux64_boot_header_file_command_line()
   );

   return;
}

void
tux64_boot_stage2_file_load_on_demand(
   Tux64BootLoadStatus load_status
) {
   if (tux64_bitwise_flags_check_one_uint8(
      load_status,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_KERNEL)
   ) == TUX64_BOOLEAN_FALSE) {
      tux64_boot_stage2_file_load_kernel();
   }
   if (tux64_bitwise_flags_check_one_uint8(
      load_status,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_INITRAMFS)
   ) == TUX64_BOOLEAN_FALSE) {
      tux64_boot_stage2_file_load_initramfs();
   }
   if (tux64_bitwise_flags_check_one_uint8(
      load_status,
      TUX64_LITERAL_UINT8(TUX64_BOOT_LOAD_STATUS_COMMAND_LINE)
   ) == TUX64_BOOLEAN_FALSE) {
      tux64_boot_stage2_file_load_command_line();
   }

   return;
}

