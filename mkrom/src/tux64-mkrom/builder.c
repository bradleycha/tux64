/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/builder.c - Implementations for ROM builder.         */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/builder.h"

#include <tux64/platform-n64/boot.h>
#include <tux64/platform-n64/rom.h>
#include <tux64/memory.h>
#include <tux64/endian.h>
#include <tux64/math.h>

/*----------------------------------------------------------------------------*/
/* Here is the ROM layout we will be using, where '.' represents the current  */
/* address:                                                                   */
/*----------------------------------------------------------------------------*/
/* 0x00000000:                                                                */
/*    ROM header                                                              */
/*                                                                            */
/* . += 0x0040:                                                               */
/*    Bootloader stage-0                                                      */
/*                                                                            */
/* . += input->files.bootloader.stage0.bytes:                                                               */
/*    Bootloader stage-0 CIC data                                             */
/*                                                                            */
/* 0x00001000:                                                                */
/*    Bootloader header                                                       */
/*                                                                            */
/* . += sizeof(struct Tux64PlatformN64BootHeader):                            */
/*    Bootloader stage-1                                                      */
/*                                                                            */
/* . += input->files.bootloader.stage1.bytes:                                 */
/*    Bootloader stage-2                                                      */
/*                                                                            */
/* . += input->files.bootloader.stage2.bytes:                                 */
/*    Bootloader stage-3                                                      */
/*                                                                            */
/* . += input->files.bootloader.stage3.bytes:                                 */
/*    Kernel image                                                            */
/*                                                                            */
/* . += input->files.kernel.bytes:                                            */
/*    Initramfs image                                                         */
/*                                                                            */
/* . += input->files.initramfs.bytes:                                         */
/*    Kernel command-line (null-terminated)                                   */
/*----------------------------------------------------------------------------*/
/* We also align all addresses to 8-byte boundaries.  RSP DMA requires        */
/* alignment to 2-byte boundaries, but we also want to use simple             */
/* implementations of memcpy in the stage-0 and stage-1 bootloaders which     */
/* operate on whole 8-byte words without concern for residual junk.           */
/*                                                                            */
/* Also note that all values must be big-endian, as that's the native         */
/* endianess of the N64.                                                      */
/*----------------------------------------------------------------------------*/

#define TUX64_MKROM_BUILDER_ALIGNMENT_BOUNDARY\
   8u
#define TUX64_MKROM_BUILDER_ALIGNMENT_MAX_VALUE\
   TUX64_LITERAL_UINT32(\
      TUX64_UINT32_MAX - (TUX64_UINT32_MAX % TUX64_MKROM_BUILDER_ALIGNMENT_BOUNDARY)\
   )

static Tux64UInt32
tux64_mkrom_builder_align_value(
   Tux64UInt32 value
) {
   return tux64_math_align_forward_uint32(
      value,
      TUX64_LITERAL_UINT32(TUX64_MKROM_BUILDER_ALIGNMENT_BOUNDARY)
   );
}

/* makes sure we don't overflow a uint32 by adding all the aligned lengths */
static struct Tux64MkromBuilderMeasureResult
tux64_mkrom_builder_measure_and_verify_overflow(
   const struct Tux64MkromBuilderInput * input
) {
   struct Tux64MkromBuilderMeasureResult result;
   Tux64UInt32 marker;
   Tux64UInt32 cmdline_bytes;

   marker = TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformN64RomHeader));

   if (marker > TUX64_MKROM_BUILDER_ALIGNMENT_MAX_VALUE - input->files.bootloader.stage0.bytes) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE0;
      return result;
   }
   marker = tux64_mkrom_builder_align_value(marker + input->files.bootloader.stage0.bytes);

   marker = tux64_mkrom_builder_align_value(marker + TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformN64BootHeader)));

   if (marker > TUX64_MKROM_BUILDER_ALIGNMENT_MAX_VALUE - input->files.bootloader.stage0_cic.bytes) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE0_CIC;
      return result;
   }
   marker = tux64_mkrom_builder_align_value(marker + input->files.bootloader.stage0_cic.bytes);

   if (marker > TUX64_MKROM_BUILDER_ALIGNMENT_MAX_VALUE - input->files.bootloader.stage1.bytes) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE1;
      return result;
   }
   marker = tux64_mkrom_builder_align_value(marker + input->files.bootloader.stage1.bytes);

   if (marker > TUX64_MKROM_BUILDER_ALIGNMENT_MAX_VALUE - input->files.bootloader.stage2.bytes) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE2;
      return result;
   }
   marker = tux64_mkrom_builder_align_value(marker + input->files.bootloader.stage2.bytes);

   if (marker > TUX64_MKROM_BUILDER_ALIGNMENT_MAX_VALUE - input->files.bootloader.stage3.bytes) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE3;
      return result;
   }
   marker = tux64_mkrom_builder_align_value(marker + input->files.bootloader.stage3.bytes);

   if (marker > TUX64_MKROM_BUILDER_ALIGNMENT_MAX_VALUE - input->files.kernel.bytes) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_KERNEL;
      return result;
   }
   marker = tux64_mkrom_builder_align_value(marker + input->files.kernel.bytes);

   if (marker > TUX64_MKROM_BUILDER_ALIGNMENT_MAX_VALUE - input->files.initramfs.bytes) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_INITRAMFS;
      return result;
   }
   marker = tux64_mkrom_builder_align_value(marker + input->files.initramfs.bytes);

   cmdline_bytes = (input->kernel_command_line.characters + TUX64_LITERAL_UINT32(1u)) * TUX64_LITERAL_UINT32(sizeof(char));

   if (cmdline_bytes > TUX64_UINT16_MAX) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_INITRAMFS;
      return result;
   }
   if (marker > TUX64_MKROM_BUILDER_ALIGNMENT_MAX_VALUE - cmdline_bytes) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_INITRAMFS;
      return result;
   }

   /* don't need to set the payload, only used as a helper function */
   result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_OK;
   return result;
}

static Tux64UInt16
tux64_mkrom_builder_store_item_uint16(
   Tux64UInt16 value
) {
   return tux64_endian_convert_uint16(value, TUX64_ENDIAN_FORMAT_BIG);
}

static Tux64UInt32
tux64_mkrom_builder_store_item_uint32(
   Tux64UInt32 value
) {
   return tux64_endian_convert_uint32(value, TUX64_ENDIAN_FORMAT_BIG);
}

static Tux64UInt16
tux64_mkrom_builder_format_length_item_uint16(
   Tux64UInt16 value
) {
   return tux64_mkrom_builder_store_item_uint16((value + TUX64_LITERAL_UINT16(8u)) / TUX64_LITERAL_UINT16(8u));
}

static Tux64UInt32
tux64_mkrom_builder_format_length_item_uint32(
   Tux64UInt32 value
) {
   return tux64_mkrom_builder_store_item_uint32((value + TUX64_LITERAL_UINT32(8u)) / TUX64_LITERAL_UINT32(8u));
}

static struct Tux64MkromBuilderMeasureResult
tux64_mkrom_builder_measure_and_verify_initialize_boot_header(
   const struct Tux64MkromBuilderInput * input,
   struct Tux64PlatformN64BootHeader * boot_header
) {
   struct Tux64MkromBuilderMeasureResult result;
   Tux64UInt32 cmdline_bytes;

   cmdline_bytes = (input->kernel_command_line.characters + TUX64_LITERAL_UINT32(1u)) * TUX64_LITERAL_UINT32(sizeof(char));

   tux64_memory_copy(
      boot_header->magic,
      TUX64_BOOT_HEADER_MAGIC,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_BOOT_HEADER_MAGIC))
   );

   boot_header->flags = tux64_mkrom_builder_store_item_uint16(0x0000);

   boot_header->version.major = TUX64_LITERAL_UINT8(0x00);
   boot_header->version.minor = TUX64_LITERAL_UINT8(0x00);
   boot_header->version.revision = tux64_mkrom_builder_store_item_uint16(0x0000);

   boot_header->length_d8 = tux64_mkrom_builder_format_length_item_uint16(TUX64_LITERAL_UINT16(sizeof(*boot_header)));

   boot_header->item_lengths_d8.kernel_data = tux64_mkrom_builder_format_length_item_uint32(input->files.kernel.bytes);
   boot_header->item_lengths_d8.initramfs = tux64_mkrom_builder_format_length_item_uint32(input->files.initramfs.bytes);
   boot_header->item_lengths_d8.command_line = tux64_mkrom_builder_format_length_item_uint16((Tux64UInt16)cmdline_bytes);
   boot_header->item_lengths_d8.bootloader_stage1 = tux64_mkrom_builder_format_length_item_uint32(input->files.bootloader.stage1.bytes);
   boot_header->item_lengths_d8.bootloader_stage2_data = tux64_mkrom_builder_format_length_item_uint32(input->files.bootloader.stage2.bytes);
   boot_header->item_lengths_d8.bootloader_stage3 = tux64_mkrom_builder_format_length_item_uint32(input->files.bootloader.stage3.bytes);

   /* TODO: parse the kernel image and stage-2 BSS data file to populate those */
   boot_header->item_lengths_d8.bootloader_stage2_bss = TUX64_LITERAL_UINT32(0u);
   boot_header->item_lengths_d8.kernel_bss = TUX64_LITERAL_UINT32(0u);

   result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_OK;
   return result;
}

#define TUX64_MKROM_BUILDER_STAGE0_MAX_LENGTH\
   (0x1000u - sizeof(struct Tux64PlatformN64RomHeader))
#define TUX64_MKROM_BUILDER_MAX_ROM_BYTES\
   (1024u * 1024u * 64u) /* 64MiB */

struct Tux64MkromBuilderMeasureResult
tux64_mkrom_builder_measure_and_verify(
   const struct Tux64MkromBuilderInput * input
) {
   struct Tux64MkromBuilderMeasureResult result;
   struct Tux64PlatformN64BootHeader boot_header;
   Tux64UInt32 marker;
   Tux64UInt32 cmdline_bytes;

   result = tux64_mkrom_builder_measure_and_verify_overflow(input);
   if (result.status != TUX64_MKROM_BUILDER_MEASURE_STATUS_OK) {
      return result;
   }

   /* initialize the boot header */
   result = tux64_mkrom_builder_measure_and_verify_initialize_boot_header(input, &boot_header);
   if (result.status != TUX64_MKROM_BUILDER_MEASURE_STATUS_OK) {
      return result;
   }

   /* verify the stage-0 binary and CIC data doesn't overflow */
   if (input->files.bootloader.stage0.bytes > TUX64_LITERAL_UINT32(TUX64_MKROM_BUILDER_STAGE0_MAX_LENGTH)) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE0;
   }
   if (input->files.bootloader.stage0.bytes + input->files.bootloader.stage0_cic.bytes > TUX64_LITERAL_UINT32(TUX64_MKROM_BUILDER_STAGE0_MAX_LENGTH)) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE0_CIC;
   }

   cmdline_bytes = (input->kernel_command_line.characters + TUX64_LITERAL_UINT32(1u)) * TUX64_LITERAL_UINT32(sizeof(char));

   /* begin measuring the size of the ROM.  we initialize to 0x1000 because */
   /* we always must have this due to the IPl2 expecting IPL3 in those bytes. */
   marker = TUX64_LITERAL_UINT32(0x00001000u);
   marker += tux64_mkrom_builder_align_value(TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformN64BootHeader)));
   
   marker += tux64_mkrom_builder_align_value(input->files.bootloader.stage1.bytes);
   if (marker > TUX64_MKROM_BUILDER_MAX_ROM_BYTES) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE1;
      return result;
   }

   marker += tux64_mkrom_builder_align_value(input->files.bootloader.stage2.bytes);
   if (marker > TUX64_MKROM_BUILDER_MAX_ROM_BYTES) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE2;
      return result;
   }

   marker += tux64_mkrom_builder_align_value(input->files.bootloader.stage3.bytes);
   if (marker > TUX64_MKROM_BUILDER_MAX_ROM_BYTES) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE3;
      return result;
   }

   marker += tux64_mkrom_builder_align_value(input->files.kernel.bytes);
   if (marker > TUX64_MKROM_BUILDER_MAX_ROM_BYTES) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_KERNEL;
      return result;
   }

   marker += tux64_mkrom_builder_align_value(input->files.initramfs.bytes);
   if (marker > TUX64_MKROM_BUILDER_MAX_ROM_BYTES) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_INITRAMFS;
      return result;
   }

   marker += tux64_mkrom_builder_align_value(cmdline_bytes);
   if (marker > TUX64_MKROM_BUILDER_MAX_ROM_BYTES) {
      result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_COMMAND_LINE;
      return result;
   }

   result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_OK;
   result.payload.ok.rom_bytes = marker;
   tux64_memory_copy(&result.payload.ok.boot_header, &boot_header, TUX64_LITERAL_UINT32(sizeof(boot_header)));
   return result;
}

void
tux64_mkrom_builder_construct(
   const struct Tux64MkromBuilderInput * input,
   const struct Tux64MkromBuilderMeasurePayloadOk * measure_info,
   Tux64UInt8 * output
) {
   Tux64UInt8 sentinel_zero;
   Tux64UInt8 * pen;

   pen = output;

   /* zero-initialize the entire ROM so we don't have to deal with residual */
   sentinel_zero = TUX64_LITERAL_UINT8(0x00);
   tux64_memory_fill(
      output,
      &sentinel_zero,
      measure_info->rom_bytes,
      TUX64_LITERAL_UINT32(sizeof(sentinel_zero))
   );

   /* ROM header */
   tux64_memory_copy(
      pen,
      &input->rom_header,
      TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformN64RomHeader))
   );
   pen += sizeof(struct Tux64PlatformN64RomHeader);

   /* stage-0 bootloader */
   tux64_memory_copy(
      pen,
      input->files.bootloader.stage0.data,
      input->files.bootloader.stage0.bytes
   );
   pen += input->files.bootloader.stage0.bytes;

   /* stage-0 bootloader CIC data */
   tux64_memory_copy(
      pen,
      input->files.bootloader.stage0_cic.data,
      input->files.bootloader.stage0_cic.bytes
   );

   pen = output + 0x1000;

   /* bootloader header */
   tux64_memory_copy(
      pen,
      &measure_info->boot_header,
      TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformN64BootHeader))
   );
   pen += tux64_mkrom_builder_align_value(TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformN64BootHeader)));

   /* stage-1 bootloader */
   tux64_memory_copy(
      pen,
      input->files.bootloader.stage1.data,
      input->files.bootloader.stage1.bytes
   );
   pen += tux64_mkrom_builder_align_value(input->files.bootloader.stage1.bytes);

   /* stage-2 bootloader */
   tux64_memory_copy(
      pen,
      input->files.bootloader.stage2.data,
      input->files.bootloader.stage2.bytes
   );
   pen += tux64_mkrom_builder_align_value(input->files.bootloader.stage2.bytes);

   /* stage-3 bootloader */
   tux64_memory_copy(
      pen,
      input->files.bootloader.stage3.data,
      input->files.bootloader.stage3.bytes
   );
   pen += tux64_mkrom_builder_align_value(input->files.bootloader.stage3.bytes);
   
   /* kernel image */
   tux64_memory_copy(
      pen,
      input->files.kernel.data,
      input->files.kernel.bytes
   );
   pen += tux64_mkrom_builder_align_value(input->files.kernel.bytes);

   /* initramfs */
   tux64_memory_copy(
      pen,
      input->files.initramfs.data,
      input->files.initramfs.bytes
   );
   pen += tux64_mkrom_builder_align_value(input->files.initramfs.bytes);

   /* kernel command-line */
   tux64_memory_copy(
      pen,
      input->kernel_command_line.ptr,
      input->kernel_command_line.characters * TUX64_LITERAL_UINT32(sizeof(char))
   );
   pen += tux64_mkrom_builder_align_value((input->kernel_command_line.characters + TUX64_LITERAL_UINT32(1u)) * TUX64_LITERAL_UINT32(sizeof(char)));

   return;
}

