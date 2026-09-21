/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/image.c - Implementations for ROM image creation.    */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/image.h"

#include <tux64/memory.h>
#include <tux64/endian.h>
#include <tux64/checksum.h>
#include <tux64/math.h>
#include <tux64/fs.h>
#include <tux64/platform/mips/n64/pi.h>
#include <tux64/platform/mips/n64/rom.h>
#include <tux64/platform/mips/n64/boot.h>
#include "tux64-mkrom/arguments.h"
#include "tux64-mkrom/load.h"

#include <stdlib.h>

#define TUX64_MKROM_IMAGE_ALIGNMENT_ADDRESS_DEFAULT\
   2u /* aligned for PI DMA */
#define TUX64_MKROM_IMAGE_ALIGNMENT_ROM_LENGTH\
   16u /* aligned for ares to not shit itself */

static Tux64UInt32
tux64_mkrom_image_checksum_endian_convert(
   Tux64UInt32 checksum
) {
   /* checksum implicitly converts to big-endian. */
   if (TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT == TUX64_ENDIAN_FORMAT_BIG) {
      return checksum;
   }

   /* byte-swap if we decide to use a little-endian boot header. */
   switch (TUX64_ENDIAN_FORMAT_NATIVE) {
      case TUX64_ENDIAN_FORMAT_BIG:
         checksum = tux64_endian_convert_uint32(checksum, TUX64_ENDIAN_FORMAT_LITTLE);
         break;
      case TUX64_ENDIAN_FORMAT_LITTLE:
         checksum = tux64_endian_convert_uint32(checksum, TUX64_ENDIAN_FORMAT_BIG);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return checksum;
}

static Tux64UInt32
tux64_mkrom_image_compute_checksum(
   const void * data,
   Tux64UInt32 bytes
) {
   struct Tux64ChecksumContext ctx;
   Tux64UInt8 * checksum;
   union Tux64Int32 output;

   tux64_checksum_fletcher_64_32.initialize(&ctx);
   tux64_checksum_fletcher_64_32.digest(&ctx, (const Tux64UInt8 *)data, bytes);
   checksum = tux64_checksum_fletcher_64_32.finalize(&ctx);

   tux64_memory_copy(
      output.bytes,
      checksum,
      TUX64_LITERAL_UINT32(TUX64_CHECKSUM_FLETCHER_64_32_DIGEST_BYTES)
   );

   return tux64_mkrom_image_checksum_endian_convert(output.uint);
}

static Tux64UInt32
tux64_mkrom_image_compute_checksum_file(
   const struct Tux64FsLoadedFile * file
) {
   return tux64_mkrom_image_compute_checksum(file->data, file->bytes);
}

static Tux64UInt32
tux64_mkrom_image_compute_checksum_command_line(
   const struct Tux64String * command_line
) {
   struct Tux64ChecksumContext ctx;
   Tux64UInt8 * checksum;
   union Tux64Int32 output;
   char null_terminator;

   null_terminator = '\0';

   /* the difference between this and the standard checksum is we have to */
   /* manually add a null terminator to our digest. */
   tux64_checksum_fletcher_64_32.initialize(&ctx);
   tux64_checksum_fletcher_64_32.digest(
      &ctx,
      (const Tux64UInt8 *)command_line->ptr,
      command_line->characters * TUX64_LITERAL_UINT32(sizeof(char))
   );
   tux64_checksum_fletcher_64_32.digest(
      &ctx,
      (const Tux64UInt8 *)&null_terminator,
      TUX64_LITERAL_UINT32(sizeof(null_terminator))
   );
   checksum = tux64_checksum_fletcher_64_32.finalize(&ctx);

   tux64_memory_copy(
      output.bytes,
      checksum,
      TUX64_LITERAL_UINT32(TUX64_CHECKSUM_FLETCHER_64_32_DIGEST_BYTES)
   );

   return tux64_mkrom_image_checksum_endian_convert(output.uint);
}

static Tux64UInt32
tux64_mkrom_image_command_line_bytes_null_terminated(
   const struct Tux64String * command_line
) {
   Tux64UInt32 characters_null_terminated;
   Tux64UInt32 bytes;

   if (command_line->characters == TUX64_LITERAL_UINT32(0u)) {
      return TUX64_LITERAL_UINT32(0u);
   }

   characters_null_terminated = command_line->characters + TUX64_LITERAL_UINT32(1u);
   bytes = characters_null_terminated * TUX64_LITERAL_UINT32(sizeof(char));

   return bytes;
}

struct Tux64MkromImageLayoutOffsets {
   Tux64UInt32 rom_header;
   Tux64UInt32 bootloader_stage0;
   Tux64UInt32 boot_header;
   Tux64UInt32 bootloader_stage1;
   Tux64UInt32 bootloader_stage2;
   Tux64UInt32 kernel;
   Tux64UInt32 initramfs;
   Tux64UInt32 rootfs;
   Tux64UInt32 command_line;
};

struct Tux64MkromImageLayout {
   struct Tux64MkromImageLayoutOffsets offsets;
   Tux64UInt32 bytes;
};

struct Tux64MkromImageLayoutIterator {
   Tux64UInt32 offset;
   Tux64UInt32 bytes;
};

static void
tux64_mkrom_image_layout_iterator_initialize(
   struct Tux64MkromImageLayoutIterator * iter
) {
   iter->offset   = TUX64_LITERAL_UINT32(0u);
   iter->bytes    = TUX64_LITERAL_UINT32(0u);
   return;
}

static Tux64Boolean
tux64_mkrom_image_layout_iterator_attach_with_alignment(
   struct Tux64MkromImageLayoutIterator * iter,
   Tux64UInt32 * output,
   Tux64UInt32 bytes,
   Tux64UInt32 align
) {
   Tux64UInt32 bytes_aligned;

   /* we need to do this so we can check for overflow without invoking */
   /* undefined behavior. */
   if (bytes > TUX64_UINT32_MAX - align + TUX64_LITERAL_UINT32(1u)) {
      return TUX64_BOOLEAN_FALSE;
   }
   if (tux64_math_ceil_divide_uint32(bytes, align) > tux64_math_ceil_divide_uint32(TUX64_UINT32_MAX, align)) {
      return TUX64_BOOLEAN_FALSE;
   }

   bytes_aligned = tux64_math_align_forward_uint32(bytes, align);

   /* once again, done carefully to avoid UB. */
   if (iter->offset > TUX64_UINT32_MAX - bytes_aligned) {
      return TUX64_BOOLEAN_FALSE;
   }

   /* eventually we will need to convert from file offsets to PI addresses, */
   /* so we need to check this for overflow as well. */
   if (iter->offset + bytes_aligned > TUX64_UINT32_MAX - TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_PI_BUS_DOMAIN_OFFSET_DOM1_ROM)) {
      return TUX64_BOOLEAN_FALSE;
   }

   *output = iter->offset;
   iter->offset += bytes_aligned;

   return TUX64_BOOLEAN_TRUE;
}

static Tux64Boolean
tux64_mkrom_image_layout_iterator_attach(
   struct Tux64MkromImageLayoutIterator * iter,
   Tux64UInt32 * output,
   Tux64UInt32 bytes
) {
   return tux64_mkrom_image_layout_iterator_attach_with_alignment(
      iter,
      output,
      bytes,
      TUX64_LITERAL_UINT32(TUX64_MKROM_IMAGE_ALIGNMENT_ADDRESS_DEFAULT)
   );
}

static Tux64Boolean
tux64_mkrom_image_layout_iterator_attach_file(
   struct Tux64MkromImageLayoutIterator * iter,
   Tux64UInt32 * output,
   const struct Tux64FsLoadedFile * file
) {
   return tux64_mkrom_image_layout_iterator_attach(iter, output, file->bytes);
}

static Tux64Boolean
tux64_mkrom_image_layout_iterator_attach_file_optional(
   struct Tux64MkromImageLayoutIterator * iter,
   Tux64UInt32 * output,
   const struct Tux64FsLoadedFile * file
) {
   if (file->data == TUX64_NULLPTR) {
      *output = iter->offset;
      return TUX64_BOOLEAN_TRUE;
   }

   return tux64_mkrom_image_layout_iterator_attach(iter, output, file->bytes);
}

static Tux64UInt32
tux64_mkrom_image_layout_iterator_bytes(
   const struct Tux64MkromImageLayoutIterator * iter
) {
   /* current offset also works as current byte length. */
   return iter->offset;
}

static struct Tux64MkromImageBuildResult
tux64_mkrom_image_build_layout(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromLoadData * data,
   struct Tux64MkromImageLayout * layout
) {
   struct Tux64MkromImageBuildResult result;
   struct Tux64MkromImageLayoutIterator iter;

   tux64_mkrom_image_layout_iterator_initialize(&iter);

   /* these first few files will never overflow, so we discard the result. */
   (void)tux64_mkrom_image_layout_iterator_attach(
      &iter,
      &layout->offsets.rom_header,
      TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformMipsN64RomHeader))
   );
   (void)tux64_mkrom_image_layout_iterator_attach(
      &iter,
      &layout->offsets.bootloader_stage0,
      TUX64_LITERAL_UINT32(TUX64_MKROM_LOAD_DATA_BOOTLOADER_STAGE0_BYTES)
   );
   (void)tux64_mkrom_image_layout_iterator_attach_with_alignment(
      &iter,
      &layout->offsets.boot_header,
      TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformMipsN64BootHeader)),
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ALIGNMENT)
   );

   /* now we must check for overflow. */
   if (tux64_mkrom_image_layout_iterator_attach_file(
      &iter,
      &layout->offsets.bootloader_stage1,
      &data->bootloader.stage1
   ) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_BOOTLOADER_STAGE1;
      return result;
   }
   if (tux64_mkrom_image_layout_iterator_attach_file(
      &iter,
      &layout->offsets.bootloader_stage2,
      &data->bootloader.stage2
   ) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_BOOTLOADER_STAGE2;
      return result;
   }
   if (tux64_mkrom_image_layout_iterator_attach(
      &iter,
      &layout->offsets.kernel,
      data->kernel.image.bytes
   ) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_KERNEL;
      return result;
   }
   if (tux64_mkrom_image_layout_iterator_attach_file_optional(
      &iter,
      &layout->offsets.initramfs,
      &data->initramfs
   ) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_INITRAMFS;
      return result;
   }
   if (tux64_mkrom_image_layout_iterator_attach_file_optional(
      &iter,
      &layout->offsets.rootfs,
      &data->rootfs
   ) == TUX64_BOOLEAN_FALSE) {
      result.status = TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_ROOTFS;
      return result;
   }
   if (config->command_line.characters != TUX64_LITERAL_UINT32(0u)) {
      if (tux64_mkrom_image_layout_iterator_attach(
         &iter,
         &layout->offsets.command_line,
         tux64_mkrom_image_command_line_bytes_null_terminated(&config->command_line)
      ) == TUX64_BOOLEAN_FALSE) {
         result.status = TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_COMMAND_LINE;
         return result;
      }
   } else {
      layout->offsets.command_line = iter.offset;
   }

   /* now make sure the final ROM image is aligned. */
   layout->bytes = tux64_math_align_forward_uint32(
      tux64_mkrom_image_layout_iterator_bytes(&iter),
      TUX64_LITERAL_UINT32(TUX64_MKROM_IMAGE_ALIGNMENT_ROM_LENGTH)
   );

   result.status = TUX64_MKROM_IMAGE_BUILD_STATUS_OK;
   return result;
}

static void
tux64_mkrom_image_build_boot_header_file(
   const Tux64UInt8 * data,
   Tux64UInt32 bytes,
   Tux64UInt32 layout_offset,
   struct Tux64PlatformMipsN64BootHeaderFile * header
) {
   Tux64UInt32 addr_cart;

   addr_cart = tux64_platform_mips_n64_pi_bus_address_dom1_rom(layout_offset);

   header->checksum  = tux64_mkrom_image_compute_checksum(data, bytes);
   header->addr_cart = tux64_endian_convert_uint32(addr_cart, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   header->length    = tux64_endian_convert_uint32(bytes, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   return;
}

static void
tux64_mkrom_image_build_boot_header_file_optional(
   const Tux64UInt8 * data,
   Tux64UInt32 bytes,
   Tux64UInt32 layout_offset,
   struct Tux64PlatformMipsN64BootHeaderFile * header
) {
   Tux64UInt32 checksum;
   Tux64UInt32 addr_cart;
   Tux64UInt32 length;

   if (data != TUX64_NULLPTR) {
      tux64_mkrom_image_build_boot_header_file(data, bytes, layout_offset, header);
      return;
   }

   /* only thing that really matters here is that 'length' is zero, as that's */
   /* how the bootloader detects and empty file.  everything else is only */
   /* initialized so that there no uninitialized data, causing UB. */
   checksum    = TUX64_LITERAL_UINT32(0u);
   addr_cart   = TUX64_LITERAL_UINT32(0u);
   length      = TUX64_LITERAL_UINT32(0u);

   header->checksum  = checksum;
   header->addr_cart = tux64_endian_convert_uint32(addr_cart, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   header->length    = tux64_endian_convert_uint32(length, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   return;
}

static void
tux64_mkrom_image_build_boot_header_files_bootloader_stage1(
   const struct Tux64MkromLoadData * data,
   struct Tux64PlatformMipsN64BootHeaderFileStage1 * stage1
) {
   Tux64UInt32 length;
   Tux64UInt32 memory;

   length = data->bootloader.stage1.bytes;
   memory = length + data->bootloader.stage1_bss;

   stage1->checksum  = tux64_mkrom_image_compute_checksum_file(&data->bootloader.stage1);
   stage1->length    = tux64_endian_convert_uint32(length, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   stage1->memory    = tux64_endian_convert_uint32(memory, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   return;
}

static void
tux64_mkrom_image_build_boot_header_files_bootloader_stage2(
   const struct Tux64MkromLoadData * data,
   const struct Tux64MkromImageLayout * layout,
   struct Tux64PlatformMipsN64BootHeaderFile * stage2
) {
   tux64_mkrom_image_build_boot_header_file(
      data->bootloader.stage2.data,
      data->bootloader.stage2.bytes,
      layout->offsets.bootloader_stage2,
      stage2
   );
   return;
}

static void
tux64_mkrom_image_build_boot_header_files_kernel(
   const struct Tux64MkromLoadData * data,
   const struct Tux64MkromImageLayout * layout,
   struct Tux64PlatformMipsN64BootHeaderFileKernel * kernel
) {
   tux64_mkrom_image_build_boot_header_file(
      &data->kernel_file.data[data->kernel.image.offset],
      data->kernel.image.bytes,
      layout->offsets.kernel,
      &kernel->image.file
   );

   kernel->image.memory = tux64_endian_convert_uint32(data->kernel.memory, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   kernel->addr_load    = tux64_endian_convert_uint32(data->kernel.addr_load, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   kernel->addr_entry   = tux64_endian_convert_uint32(data->kernel.addr_entry, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   return;
}

static void
tux64_mkrom_image_build_boot_header_files_initramfs(
   const struct Tux64MkromLoadData * data,
   const struct Tux64MkromImageLayout * layout,
   struct Tux64PlatformMipsN64BootHeaderFile * initramfs
) {
   tux64_mkrom_image_build_boot_header_file_optional(
      data->initramfs.data,
      data->initramfs.bytes,
      layout->offsets.initramfs,
      initramfs
   );
   return;
}

static void
tux64_mkrom_image_build_boot_header_files_rootfs(
   const struct Tux64MkromLoadData * data,
   const struct Tux64MkromImageLayout * layout,
   struct Tux64PlatformMipsN64BootHeaderFileRootfs * rootfs
) {
   Tux64UInt32 addr_cart;
   Tux64UInt32 length;
   Tux64UInt32 empty;

   if (data->rootfs.data == TUX64_NULLPTR) {
      empty = tux64_endian_convert_uint32(TUX64_LITERAL_UINT32(0u), TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);

      rootfs->addr_cart = empty;
      rootfs->length    = empty;
      return;
   }

   addr_cart   = tux64_platform_mips_n64_pi_bus_address_dom1_rom(layout->offsets.rootfs);
   length      = data->rootfs.bytes;

   rootfs->addr_cart = tux64_endian_convert_uint32(addr_cart, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   rootfs->length    = tux64_endian_convert_uint32(length, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   return;
}

static void
tux64_mkrom_image_build_boot_header_files_command_line(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromImageLayout * layout,
   struct Tux64PlatformMipsN64BootHeaderFile * command_line
) {
   Tux64UInt32 addr_cart;
   Tux64UInt32 length;
   Tux64UInt32 empty;

   if (config->command_line.characters == TUX64_LITERAL_UINT32(0u)) {
      empty = tux64_endian_convert_uint32(TUX64_LITERAL_UINT32(0u), TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);

      command_line->checksum  = empty;
      command_line->addr_cart = empty;
      command_line->length    = empty;
      return;
   }

   addr_cart   = tux64_platform_mips_n64_pi_bus_address_dom1_rom(layout->offsets.command_line);
   length      = tux64_mkrom_image_command_line_bytes_null_terminated(&config->command_line);

   command_line->checksum  = tux64_mkrom_image_compute_checksum_command_line(&config->command_line);
   command_line->addr_cart = tux64_endian_convert_uint32(addr_cart, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   command_line->length    = tux64_endian_convert_uint32(length, TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT);
   return;
}

static void
tux64_mkrom_image_build_boot_header_files(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromLoadData * data,
   const struct Tux64MkromImageLayout * layout,
   struct Tux64PlatformMipsN64BootHeaderFiles * boot_header_files
) {
   tux64_mkrom_image_build_boot_header_files_bootloader_stage1(
      data,
      &boot_header_files->bootloader.stage1
   );
   tux64_mkrom_image_build_boot_header_files_bootloader_stage2(
      data,
      layout,
      &boot_header_files->bootloader.stage2
   );
   tux64_mkrom_image_build_boot_header_files_kernel(
      data,
      layout,
      &boot_header_files->kernel
   );
   tux64_mkrom_image_build_boot_header_files_initramfs(
      data,
      layout,
      &boot_header_files->initramfs
   );
   tux64_mkrom_image_build_boot_header_files_rootfs(
      data,
      layout,
      &boot_header_files->rootfs
   );
   tux64_mkrom_image_build_boot_header_files_command_line(
      config,
      layout,
      &boot_header_files->command_line
   );

   return;
}

static void
tux64_mkrom_image_build_boot_header_data(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromLoadData * data,
   const struct Tux64MkromImageLayout * layout,
   struct Tux64PlatformMipsN64BootHeaderData * boot_header_data
) {
   boot_header_data->flags = tux64_endian_convert_uint32(
      data->bootloader.flags,
      TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT
   );
   
   tux64_mkrom_image_build_boot_header_files(
      config,
      data,
      layout,
      &boot_header_data->files
   );

   return;
}

static void
tux64_mkrom_image_build_boot_header(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromLoadData * data,
   const struct Tux64MkromImageLayout * layout,
   struct Tux64PlatformMipsN64BootHeader * boot_header
) {
   union Tux64Int32 magic;

   tux64_mkrom_image_build_boot_header_data(
      config,
      data,
      layout,
      &boot_header->data
   );

   magic.uint = tux64_endian_convert_uint32(
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_MAGIC),
      TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_ENDIAN_FORMAT
   );

   tux64_memory_copy(
      boot_header->magic,
      magic.bytes,
      TUX64_LITERAL_UINT32(sizeof(boot_header->magic))
   );

   boot_header->checksum = tux64_mkrom_image_compute_checksum(
      &boot_header->data,
      TUX64_LITERAL_UINT32(sizeof(boot_header->data))
   );
   
   return;
}

static void
tux64_mkrom_image_build_binary_allocated(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromLoadData * data,
   const struct Tux64MkromImageLayoutOffsets * offsets,
   const struct Tux64PlatformMipsN64BootHeader * boot_header,
   struct Tux64MkromImage * image
) {
   Tux64UInt8 zero_sentinel;

   /* we zero-initialize the entire ROM image so that unused bytes which are */
   /* a result of alignment get filled in.  this also takes care of */
   /* null-terminating the kernel command-line. */
   zero_sentinel = TUX64_LITERAL_UINT8(0u);
   tux64_memory_fill(
      image->data,
      &zero_sentinel,
      image->bytes,
      TUX64_LITERAL_UINT32(1u)
   );

   /* we now iteratively copy every boot item into the ROM image.  note that */
   /* endinaness is handled by previous code, so it's safe to use direct */
   /* memory copies. */
   tux64_memory_copy(
      &image->data[offsets->rom_header],
      &config->rom_header,
      TUX64_LITERAL_UINT32(sizeof(config->rom_header))
   );
   tux64_memory_copy(
      &image->data[offsets->bootloader_stage0],
      data->bootloader.stage0,
      TUX64_LITERAL_UINT32(sizeof(data->bootloader.stage0))
   );
   tux64_memory_copy(
      &image->data[offsets->boot_header],
      boot_header,
      TUX64_LITERAL_UINT32(sizeof(*boot_header))
   );
   tux64_memory_copy(
      &image->data[offsets->bootloader_stage1],
      data->bootloader.stage1.data,
      data->bootloader.stage1.bytes
   );
   tux64_memory_copy(
      &image->data[offsets->bootloader_stage2],
      data->bootloader.stage2.data,
      data->bootloader.stage2.bytes
   );
   tux64_memory_copy(
      &image->data[offsets->kernel],
      &data->kernel_file.data[data->kernel.image.offset],
      data->kernel.image.bytes
   );
   if (data->initramfs.data != TUX64_NULLPTR) {
      tux64_memory_copy(
         &image->data[offsets->initramfs],
         data->initramfs.data,
         data->initramfs.bytes
      );
   }
   if (data->rootfs.data != TUX64_NULLPTR) {
      tux64_memory_copy(
         &image->data[offsets->rootfs],
         data->rootfs.data,
         data->rootfs.bytes
      );
   }
   if (config->command_line.characters != TUX64_LITERAL_UINT32(0u)) {
      tux64_memory_copy(
         &image->data[offsets->command_line],
         config->command_line.ptr,
         config->command_line.characters * TUX64_LITERAL_UINT32(sizeof(char))
      );
   }

   return;
}

static struct Tux64MkromImageBuildResult
tux64_mkrom_image_build_binary(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromLoadData * data,
   const struct Tux64MkromImageLayout * layout,
   const struct Tux64PlatformMipsN64BootHeader * boot_header
) {
   struct Tux64MkromImageBuildResult result;
   struct Tux64MkromImage image;

   image.data = (Tux64UInt8 *)malloc(layout->bytes);
   if (image.data == NULL) {
      result.status = TUX64_MKROM_IMAGE_BUILD_STATUS_OUT_OF_MEMORY;
      return result;
   }
   image.bytes = layout->bytes;

   tux64_mkrom_image_build_binary_allocated(
      config,
      data,
      &layout->offsets,
      boot_header,
      &image
   );

   result.status = TUX64_MKROM_IMAGE_BUILD_STATUS_OK;
   result.payload.ok = image;
   return result;
}

struct Tux64MkromImageBuildResult
tux64_mkrom_image_build(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromLoadData * data
) {
   struct Tux64MkromImageBuildResult result;
   struct Tux64MkromImageLayout layout;
   struct Tux64PlatformMipsN64BootHeader boot_header;

   result = tux64_mkrom_image_build_layout(config, data, &layout);
   if (result.status != TUX64_MKROM_IMAGE_BUILD_STATUS_OK) {
      return result;
   }

   tux64_mkrom_image_build_boot_header(config, data, &layout, &boot_header);

   return tux64_mkrom_image_build_binary(config, data, &layout, &boot_header);
}

void
tux64_mkrom_image_free(
   struct Tux64MkromImage * image
) {
   free(image->data);
   return;
}

