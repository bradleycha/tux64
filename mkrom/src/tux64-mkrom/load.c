/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/load.c - Implementations for boot file loading and   */
/*    parsing.                                                                */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/load.h"

#include <tux64/fs.h>
#include <tux64/log.h>
#include <tux64/memory.h>
#include <tux64/bitwise.h>
#include <tux64/string.h>
#include <tux64/parse/string-integer.h>
#include <tux64/platform/mips/n64/boot.h>
#include <tux64/platform/mips/n64/kernel.h>
#include "tux64-mkrom/file.h"

#include <inttypes.h>

#define TUX64_MKROM_LOAD_N64_MAXIMUM_MEMORY\
   (8u * 1024u * 1024u) /* 8MiB */

static struct Tux64FsFileLoadResult
tux64_mkrom_load_file_with_log_message(
   const struct Tux64String * path,
   const struct Tux64String * prefix,
   const char * name
) {
   TUX64_LOG_INFO_FMT(
      "reading %s from %.*s%.*s",
      name,
      prefix->characters,
      prefix->ptr,
      path->characters,
      path->ptr
   );

   return tux64_mkrom_file_load_from_prefix(path, prefix);
}

static Tux64Boolean
tux64_mkrom_load_check_for_n64_memory_overflow(
   const char * identifier,
   Tux64UInt32 value
) {
   if (value <= TUX64_LITERAL_UINT32(TUX64_MKROM_LOAD_N64_MAXIMUM_MEMORY)) {
      return TUX64_BOOLEAN_FALSE;
   }

   TUX64_LOG_WARNING_FMT(
      "%s of %" PRIu32 " bytes exceeds maximum possible N64 memory of 8MiB by %" PRIu32 " bytes",
      identifier,
      value,
      value - TUX64_LITERAL_UINT32(TUX64_MKROM_LOAD_N64_MAXIMUM_MEMORY)
   );

   return TUX64_BOOLEAN_TRUE;
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_format_fs_error(
   struct Tux64FsFileLoadResult reason
) {
   struct Tux64MkromLoadResult result;

   switch (reason.status) {
      case TUX64_FS_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_MKROM_LOAD_STATUS_OUT_OF_MEMORY;
         break;
      default:
         result.status = TUX64_MKROM_LOAD_STATUS_FS_ERROR;
         result.payload.fs_error.reason = tux64_mkrom_file_load_result_to_fs_result(reason);
         break;
   }

   return result;
}

typedef struct Tux64MkromLoadResult (*Tux64MkromLoadDataPfn)(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
);

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_file(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data,
   const char * name,
   const struct Tux64String * path,
   struct Tux64FsLoadedFile * output,
   Tux64MkromLoadDataPfn next
) {
   struct Tux64MkromLoadResult result;
   struct Tux64FsFileLoadResult load_result;

   load_result = tux64_mkrom_load_file_with_log_message(
      path,
      file_path_prefix,
      name
   );
   if (load_result.status != TUX64_FS_STATUS_OK) {
      return tux64_mkrom_load_format_fs_error(load_result);
   }

   *output = load_result.payload.ok;

   result = next(config, file_path_prefix, load_data);
   if (result.status != TUX64_MKROM_LOAD_STATUS_OK) {
      tux64_fs_file_unload(&load_result.payload.ok);
   }
   return result;
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_file_optional(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data,
   const char * name,
   const struct Tux64String * path,
   struct Tux64FsLoadedFile * output,
   Tux64MkromLoadDataPfn next
) {
   if (path->characters == TUX64_LITERAL_UINT32(0u)) {
      output->data = TUX64_NULLPTR;
      return next(config, file_path_prefix, load_data);
   }

   return tux64_mkrom_load_data_file(
      config,
      file_path_prefix,
      load_data,
      name,
      path,
      output,
      next
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_complete(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   struct Tux64MkromLoadResult result;

   /* woo-hoo! we did it! */
   (void)config;
   (void)file_path_prefix;
   (void)load_data;
   result.status = TUX64_MKROM_LOAD_STATUS_OK;
   return result;
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_rootfs_sanity_checks(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   /* in the future, we may add sanity checks.  but for now, we just continue. */
   return tux64_mkrom_load_data_complete(
      config,
      file_path_prefix,
      load_data
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_rootfs(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   return tux64_mkrom_load_data_file_optional(
      config,
      file_path_prefix,
      load_data,
      "rootfs image",
      &config->path_rootfs,
      &load_data->rootfs,
      tux64_mkrom_load_data_rootfs_sanity_checks
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_initramfs_sanity_checks(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   tux64_mkrom_load_check_for_n64_memory_overflow(
      "initramfs image",
      load_data->initramfs.bytes
   );

   return tux64_mkrom_load_data_rootfs(
      config,
      file_path_prefix,
      load_data
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_initramfs(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   return tux64_mkrom_load_data_file_optional(
      config,
      file_path_prefix,
      load_data,
      "initramfs image",
      &config->path_initramfs,
      &load_data->initramfs,
      tux64_mkrom_load_data_initramfs_sanity_checks
   );
}

static void
tux64_mkrom_load_data_kernel_auxdata(
   const struct Tux64PlatformMipsN64Kernel * kernel,
   struct Tux64MkromLoadData * load_data
) {
   if (kernel->endian_format == TUX64_ENDIAN_FORMAT_LITTLE) {
      load_data->bootloader.flags = tux64_bitwise_flags_set_uint32(
         load_data->bootloader.flags,
         TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_KERNEL_ENDIAN_FORMAT_LITTLE)
      );
   }

   return;
}

static void
tux64_mkrom_load_data_kernel_sanity_checks(
   const struct Tux64PlatformMipsN64Kernel * kernel
) {
   (void)tux64_mkrom_load_check_for_n64_memory_overflow(
      "kernel code",
      kernel->image.bytes
   );
   (void)tux64_mkrom_load_check_for_n64_memory_overflow(
      "kernel total memory",
      kernel->memory
   );

   return;
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_kernel(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   struct Tux64MkromLoadResult result;
   struct Tux64PlatformMipsN64KernelParseResult kernel_parse_result;

   kernel_parse_result = tux64_platform_mips_n64_kernel_parse(
      load_data->kernel_file.data,
      load_data->kernel_file.bytes
   );
   if (kernel_parse_result.status != TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK) {
      result.status = TUX64_MKROM_LOAD_STATUS_KERNEL_PARSE_ERROR;
      result.payload.kernel_parse_error.reason = kernel_parse_result;
      return result;
   }

   load_data->kernel = kernel_parse_result.payload.ok;

   tux64_mkrom_load_data_kernel_auxdata(&kernel_parse_result.payload.ok, load_data);
   tux64_mkrom_load_data_kernel_sanity_checks(&kernel_parse_result.payload.ok);

   return tux64_mkrom_load_data_initramfs(
      config,
      file_path_prefix,
      load_data
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_kernel_file(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   return tux64_mkrom_load_data_file(
      config,
      file_path_prefix,
      load_data,
      "kernel ELF image",
      &config->path_kernel,
      &load_data->kernel_file,
      tux64_mkrom_load_data_kernel
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage2_sanity_checks(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   (void)tux64_mkrom_load_check_for_n64_memory_overflow(
      "bootloader stage-2 code",
      load_data->bootloader.stage2.bytes
   );

   return tux64_mkrom_load_data_kernel_file(
      config,
      file_path_prefix,
      load_data
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage2(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   return tux64_mkrom_load_data_file(
      config,
      file_path_prefix,
      load_data,
      "bootloader stage-2 code",
      &config->path_bootloader_stage2,
      &load_data->bootloader.stage2,
      tux64_mkrom_load_data_stage2_sanity_checks
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage1_bss_parse(
   struct Tux64MkromLoadData * load_data,
   const struct Tux64FsLoadedFile * file
) {
   struct Tux64MkromLoadResult result;
   struct Tux64ParseStringIntegerResult parse_result;
   struct Tux64String bss_string;
   struct Tux64String bss_string_trimmed;
   Tux64UInt32 bss;
   Tux64UInt32 stage1_memory_usage;

   bss_string.ptr = (const char *)file->data;
   bss_string.characters = file->bytes / TUX64_LITERAL_UINT32(sizeof(char));

   /* we need this since there can be newlines and other whitespace which may */
   /* cause issues.  this results in less strictness with the parser. */
   bss_string_trimmed = tux64_string_trim_whitespace(&bss_string);

   parse_result = tux64_parse_string_integer_hex_uint32(&bss_string_trimmed, &bss);
   if (parse_result.status != TUX64_PARSE_STRING_INTEGER_STATUS_OK) {
      result.status = TUX64_MKROM_LOAD_STATUS_INVALID_STAGE1_BSS;
      result.payload.invalid_stage1_bss.reason= parse_result;
      return result;
   }

   /* need two seperate checks to prevent possible integer overflow. */
   if (tux64_mkrom_load_check_for_n64_memory_overflow(
      "bootloader stage-1 BSS memory",
      bss
   ) == TUX64_BOOLEAN_FALSE) {
      /* this isn't entirely accurate since we don't account for the memory */
      /* layout, but this is close enough.  if you're right on the limit, */
      /* you have bigger issues than memory usage. */
      stage1_memory_usage = load_data->bootloader.stage1.bytes + bss;
      (void)tux64_mkrom_load_check_for_n64_memory_overflow(
         "bootloader stage-1 total memory",
         stage1_memory_usage
      );
   }

   load_data->bootloader.stage1_bss = bss;

   result.status = TUX64_MKROM_LOAD_STATUS_OK;
   return result;
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage1_bss(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   struct Tux64MkromLoadResult result;
   struct Tux64FsFileLoadResult load_result;

   load_result = tux64_mkrom_load_file_with_log_message(
      &config->path_bootloader_stage1_bss,
      file_path_prefix,
      "bootloader stage-1 BSS memory usage"
   );
   if (load_result.status != TUX64_FS_STATUS_OK) {
      return tux64_mkrom_load_format_fs_error(load_result);
   }

   result = tux64_mkrom_load_data_stage1_bss_parse(
      load_data,
      &load_result.payload.ok
   );
   tux64_fs_file_unload(&load_result.payload.ok);
   if (result.status != TUX64_MKROM_LOAD_STATUS_OK) {
      return result;
   }

   return tux64_mkrom_load_data_stage2(
      config,
      file_path_prefix,
      load_data
   );
}

/* we have to make sure the bootloader is 4-byte aligned because the stage-0 */
/* checksum implementation will break for unaligned data.  the rest of the */
/* stages use tux64-lib's checksum implementation, and thus work fine with */
/* unaligned data. */
#define TUX64_MKROM_LOAD_STAGE1_ALIGNMENT\
   4u

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage1_check_alignment(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   struct Tux64MkromLoadResult result;
   Tux64UInt32 length;
   Tux64UInt32 alignment;
   Tux64UInt32 residual;

   length      = load_data->bootloader.stage1.bytes;
   alignment   = TUX64_LITERAL_UINT32(TUX64_MKROM_LOAD_STAGE1_ALIGNMENT);
   residual    = length % alignment;

   if (residual != TUX64_LITERAL_UINT32(0u)) {
      result.status = TUX64_MKROM_LOAD_STATUS_INVALID_STAGE1_ALIGNMENT;
      result.payload.invalid_stage1_alignment.length_provided = length;
      result.payload.invalid_stage1_alignment.alignment = alignment;
      result.payload.invalid_stage1_alignment.residual = residual;
      return result;
   }

   return tux64_mkrom_load_data_stage1_bss(
      config,
      file_path_prefix,
      load_data
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage1(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   return tux64_mkrom_load_data_file(
      config,
      file_path_prefix,
      load_data,
      "bootloader stage-1 code",
      &config->path_bootloader_stage1,
      &load_data->bootloader.stage1,
      tux64_mkrom_load_data_stage1_check_alignment
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage0_cic_verify_and_copy(
   struct Tux64MkromLoadData * load_data,
   const struct Tux64FsLoadedFile * file
) {
   struct Tux64MkromLoadResult result;
   Tux64UInt32 length_expected;

   length_expected = TUX64_LITERAL_UINT32(TUX64_MKROM_LOAD_DATA_BOOTLOADER_STAGE0_CIC_BYTES);

   if (file->bytes != length_expected) {
      result.status = TUX64_MKROM_LOAD_STATUS_INVALID_STAGE0_CIC_LENGTH;
      result.payload.invalid_stage0_cic_length.length_provided = file->bytes;
      result.payload.invalid_stage0_cic_length.length_expected = length_expected;
      return result;
   }

   tux64_memory_copy(
      &load_data->bootloader.stage0[TUX64_MKROM_LOAD_DATA_BOOTLOADER_STAGE0_BYTES - length_expected],
      file->data,
      length_expected
   );

   result.status = TUX64_MKROM_LOAD_STATUS_OK;
   return result;
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage0_cic(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   struct Tux64MkromLoadResult result;
   struct Tux64FsFileLoadResult load_result;

   load_result = tux64_mkrom_load_file_with_log_message(
      &config->path_bootloader_stage0_cic,
      file_path_prefix,
      "bootloader stage-0 CIC data"
   );
   if (load_result.status != TUX64_FS_STATUS_OK) {
      return tux64_mkrom_load_format_fs_error(load_result);
   }

   result = tux64_mkrom_load_data_stage0_cic_verify_and_copy(
      load_data,
      &load_result.payload.ok
   );
   tux64_fs_file_unload(&load_result.payload.ok);
   if (result.status != TUX64_MKROM_LOAD_STATUS_OK) {
      return result;
   }

   return tux64_mkrom_load_data_stage1(
      config,
      file_path_prefix,
      load_data
   );
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage0_verify_and_copy(
   struct Tux64MkromLoadData * load_data,
   const struct Tux64FsLoadedFile * file
) {
   struct Tux64MkromLoadResult result;
   Tux64UInt32 length_maximum;
   Tux64UInt8 zero_sentinel;

   length_maximum = TUX64_LITERAL_UINT32(
      TUX64_MKROM_LOAD_DATA_BOOTLOADER_STAGE0_BYTES
      - TUX64_MKROM_LOAD_DATA_BOOTLOADER_STAGE0_CIC_BYTES
   );

   if (file->bytes > length_maximum) {
      result.status = TUX64_MKROM_LOAD_STATUS_INVALID_STAGE0_LENGTH;
      result.payload.invalid_stage0_length.length_provided = file->bytes;
      result.payload.invalid_stage0_length.length_maximum = length_maximum;
      return result;
   }

   tux64_memory_copy(
      load_data->bootloader.stage0,
      file->data,
      file->bytes
   );

   zero_sentinel = TUX64_LITERAL_UINT8(0u);
   tux64_memory_fill(
      &load_data->bootloader.stage0[file->bytes],
      &zero_sentinel,
      length_maximum - file->bytes,
      TUX64_LITERAL_UINT32(1u)
   );

   result.status = TUX64_MKROM_LOAD_STATUS_OK;
   return result;
}

static struct Tux64MkromLoadResult
tux64_mkrom_load_data_stage0(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix,
   struct Tux64MkromLoadData * load_data
) {
   struct Tux64MkromLoadResult result;
   struct Tux64FsFileLoadResult load_result;

   load_result = tux64_mkrom_load_file_with_log_message(
      &config->path_bootloader_stage0,
      file_path_prefix,
      "bootloader stage-0 code"
   );
   if (load_result.status != TUX64_FS_STATUS_OK) {
      return tux64_mkrom_load_format_fs_error(load_result);
   }

   result = tux64_mkrom_load_data_stage0_verify_and_copy(
      load_data,
      &load_result.payload.ok
   );
   tux64_fs_file_unload(&load_result.payload.ok);
   if (result.status != TUX64_MKROM_LOAD_STATUS_OK) {
      return result;
   }

   return tux64_mkrom_load_data_stage0_cic(
      config,
      file_path_prefix,
      load_data
   );
}

struct Tux64MkromLoadResult
tux64_mkrom_load_data(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix
) {
   struct Tux64MkromLoadResult result;
   struct Tux64MkromLoadData load_data;

   /* we initialize this immediately because while most of the boot flags are */
   /* set from the configuration, boot files can implicitly set additional */
   /* boot flags (ex: kernel endianess). */
   load_data.bootloader.flags = config->boot_header_flags;

   /* we use the same design pattern from tux64-mkrom.c to easily unwind */
   /* allocations in the event of an error.  therefore, calling this function */
   /* also loads everything sequentially. */
   result = tux64_mkrom_load_data_stage0(
      config,
      file_path_prefix,
      &load_data
   );
   if (result.status != TUX64_MKROM_LOAD_STATUS_OK) {
      return result;
   }

   /* great success! */
   result.status = TUX64_MKROM_LOAD_STATUS_OK;
   result.payload.ok = load_data;
   return result;
}

static void
tux64_mkrom_load_free_data_optional_file(
   struct Tux64FsLoadedFile * file
) {
   if (file->data == TUX64_NULLPTR) {
      return;
   }

   tux64_fs_file_unload(file);
   return;
}

void
tux64_mkrom_load_free_data(
   struct Tux64MkromLoadData * data
) {
   tux64_mkrom_load_free_data_optional_file(&data->rootfs);
   tux64_mkrom_load_free_data_optional_file(&data->initramfs);
   tux64_fs_file_unload(&data->kernel_file);
   tux64_fs_file_unload(&data->bootloader.stage2);
   tux64_fs_file_unload(&data->bootloader.stage1);
   return;
}

