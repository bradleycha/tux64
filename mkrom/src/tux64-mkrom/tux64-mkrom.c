/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/tux64-mkrom.c - Main application entrypoint for      */
/*    tux64-mkrom.                                                            */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/arguments.h"
#include "tux64-mkrom/file.h"
#include "tux64-mkrom/load.h"
#include "tux64-mkrom/image.h"
#include <tux64/log.h>
#include <tux64/arguments.h>
#include <tux64/fs.h>
#include <tux64/string.h>
#include <tux64/parse/string-integer.h>
#include <tux64/platform/mips/n64/kernel.h>

#include <inttypes.h>

#define TUX64_MKROM_ARGC_MAX \
   TUX64_UINT8_MAX

enum Tux64MkromExitStatus {
   TUX64_MKROM_EXIT_STATUS_OK = 0u,
   TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY,
   TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS,
   TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR,
   TUX64_MKROM_EXIT_STATUS_FS_ERROR,
   TUX64_MKROM_EXIT_STATUS_LOAD_ERROR,
   TUX64_MKROM_EXIT_STATUS_IMAGE_BUILD_ERROR
};

struct Tux64MkromExitPayloadOutOfMemory {
   const char * allocation_name;
};

struct Tux64MkromExitPayloadTooManyArguments {
   int argc_given;
   Tux64UInt8 argc_max;
};

struct Tux64MkromExitPayloadFsError {
   struct Tux64FsResult reason;
};

struct Tux64MkromExitPayloadLoadError {
   struct Tux64MkromLoadResult reason;
};

struct Tux64MkromExitPayloadImageBuildError {
   struct Tux64MkromImageBuildResult reason;
};

union Tux64MkromExitPayload {
   struct Tux64MkromExitPayloadOutOfMemory out_of_memory;
   struct Tux64MkromExitPayloadTooManyArguments too_many_arguments;
   struct Tux64MkromExitPayloadFsError fs_error;
   struct Tux64MkromExitPayloadLoadError load_error;
   struct Tux64MkromExitPayloadImageBuildError image_build_error;
};

struct Tux64MkromExitResult {
   enum Tux64MkromExitStatus status;
   union Tux64MkromExitPayload payload;
};

static void
tux64_mkrom_exit_result_display_out_of_memory(
   const struct Tux64MkromExitPayloadOutOfMemory * self
) {
   TUX64_LOG_ERROR_FMT(
      "ran out of memory during %s",
      self->allocation_name
   );
}

static void
tux64_mkrom_exit_result_display_too_many_arguments(
   const struct Tux64MkromExitPayloadTooManyArguments * self
) {
   TUX64_LOG_ERROR_FMT(
      "unable to handle %d arguments at once, maximum is %" PRIu8,
      self->argc_given,
      self->argc_max
   );
}

static void
tux64_mkrom_exit_result_display_bad_alignment(
   const char * name,
   Tux64UInt32 length_provided,
   Tux64UInt32 alignment,
   Tux64UInt32 residual
) {
   TUX64_LOG_ERROR_FMT(
      "%s of %" PRIu32 " bytes must be aligned to a %" PRIu32 "-byte boundary, however it is misaligned by %" PRIu32 " bytes",
      name,
      length_provided,
      alignment,
      residual
   );

   return;
}

static void
tux64_mkrom_exit_result_display_load_error_invalid_stage0_length(
   const struct Tux64MkromLoadPayloadInvalidStage0Length * invalid_stage0_length
) {
   TUX64_LOG_ERROR_FMT(
      "stage-0 bootloader binary must be less than %" PRIu32 " bytes, but provided binary is %" PRIu32 " bytes",
      invalid_stage0_length->length_maximum,
      invalid_stage0_length->length_provided
   );
   return;
}

static void
tux64_mkrom_exit_result_display_load_error_invalid_stage0_cic_length(
   const struct Tux64MkromLoadPayloadInvalidStage0CicLength * invalid_stage0_cic_length
) {
   TUX64_LOG_ERROR_FMT(
      "expected stage-0 bootloader CIC data to be %" PRIu32 " bytes, but provided data is %" PRIu32 " bytes",
      invalid_stage0_cic_length->length_expected,
      invalid_stage0_cic_length->length_provided
   );
   return;
}

static void
tux64_mkrom_exit_result_display_load_error_invalid_stage1_bss_hex_digit(
   char c
) {
   switch (tux64_string_character_is_alphanumeric(c)) {
      case TUX64_BOOLEAN_TRUE:
         TUX64_LOG_ERROR_FMT(
            "stage-1 bootloader BSS length contains invalid hex digit \'%c\'",
            c
         );
         break;
      case TUX64_BOOLEAN_FALSE:
         TUX64_LOG_ERROR_FMT(
            "stage-1 bootloader BSS length contains invalid hex digit code %" PRIu8 "/0x%02" PRIx8,
            c,
            (Tux64UInt8)c
         );
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_mkrom_exit_result_display_load_error_invalid_stage1_alignment(
   const struct Tux64MkromLoadPayloadInvalidStage1Alignment * invalid_stage1_alignment
) {
   tux64_mkrom_exit_result_display_bad_alignment(
      "stage-1 bootloader",
      invalid_stage1_alignment->length_provided,
      invalid_stage1_alignment->alignment,
      invalid_stage1_alignment->residual
   );

   return;
}

static void
tux64_mkrom_exit_result_display_load_error_invalid_stage1_bss(
   const struct Tux64MkromLoadPayloadInvalidStage1Bss * invalid_stage1_bss
) {
   switch (invalid_stage1_bss->reason.status) {
      case TUX64_PARSE_STRING_INTEGER_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_PARSE_STRING_INTEGER_STATUS_INVALID_DIGIT:
         tux64_mkrom_exit_result_display_load_error_invalid_stage1_bss_hex_digit(invalid_stage1_bss->reason.payload.invalid_digit.character);
         break;
      case TUX64_PARSE_STRING_INTEGER_STATUS_OUT_OF_RANGE:
         TUX64_LOG_ERROR("stage-1 bootloader BSS length is too large");
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_mkrom_exit_result_display_load_error_kernel_parse_error(
   const struct Tux64MkromLoadPayloadKernelParseError * kernel_parse_error
) {
   switch (kernel_parse_error->reason.status) {
      case TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE:
         TUX64_LOG_ERROR("kernel image is either corrupted or malformed");
         break;
      case TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_BAD_VERSION:
         TUX64_LOG_ERROR_FMT(
            "kernel image has an invalid ELF version 0x%08" PRIx32,
            kernel_parse_error->reason.payload.bad_version.version
         );
         break;
      case TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_INVALID_TYPE:
         TUX64_LOG_ERROR_FMT(
            "kernel image has an invalid ELF object type 0x%08" PRIx32,
            kernel_parse_error->reason.payload.invalid_type.type
         );
         break;
      case TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_INVALID_MACHINE:
         TUX64_LOG_ERROR_FMT(
            "kernel image has an invalid target machine 0x%08" PRIx32,
            kernel_parse_error->reason.payload.invalid_machine.machine
         );
         break;
      case TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_MISSING:
         TUX64_LOG_ERROR("no executable code segment was found in the kernel image");
         break;
      case TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_DUPLICATE:
         TUX64_LOG_ERROR("multiple executable code segments were found in the kernel image");
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_mkrom_exit_result_display_load_error_invalid_rootfs_alignment(
   const struct Tux64MkromLoadPayloadInvalidRootfsAlignment * invalid_rootfs_alignment
) {
   tux64_mkrom_exit_result_display_bad_alignment(
      "rootfs image",
      invalid_rootfs_alignment->length_provided,
      invalid_rootfs_alignment->alignment,
      invalid_rootfs_alignment->residual
   );

   return;
}

static void
tux64_mkrom_exit_result_display_load_error(
   const struct Tux64MkromExitPayloadLoadError * self
) {
   switch (self->reason.status) {
      case TUX64_MKROM_LOAD_STATUS_OK:
      case TUX64_MKROM_LOAD_STATUS_OUT_OF_MEMORY:
      case TUX64_MKROM_LOAD_STATUS_FS_ERROR:
         TUX64_UNREACHABLE;
      case TUX64_MKROM_LOAD_STATUS_INVALID_STAGE0_LENGTH:
         tux64_mkrom_exit_result_display_load_error_invalid_stage0_length(&self->reason.payload.invalid_stage0_length);
         break;
      case TUX64_MKROM_LOAD_STATUS_INVALID_STAGE0_CIC_LENGTH:
         tux64_mkrom_exit_result_display_load_error_invalid_stage0_cic_length(&self->reason.payload.invalid_stage0_cic_length);
         break;
      case TUX64_MKROM_LOAD_STATUS_INVALID_STAGE1_ALIGNMENT:
         tux64_mkrom_exit_result_display_load_error_invalid_stage1_alignment(&self->reason.payload.invalid_stage1_alignment);
         break;
      case TUX64_MKROM_LOAD_STATUS_INVALID_STAGE1_BSS:
         tux64_mkrom_exit_result_display_load_error_invalid_stage1_bss(&self->reason.payload.invalid_stage1_bss);
         break;
      case TUX64_MKROM_LOAD_STATUS_KERNEL_PARSE_ERROR:
         tux64_mkrom_exit_result_display_load_error_kernel_parse_error(&self->reason.payload.kernel_parse_error);
         break;
      case TUX64_MKROM_LOAD_STATUS_INVALID_ROOTFS_ALIGNMENT:
         tux64_mkrom_exit_result_display_load_error_invalid_rootfs_alignment(&self->reason.payload.invalid_rootfs_alignment);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}
   
static void
tux64_mkrom_exit_result_display_image_build_error(
   const struct Tux64MkromExitPayloadImageBuildError * self
) {
   const char * offender;

   switch (self->reason.status) {
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OK:
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OUT_OF_MEMORY:
         TUX64_UNREACHABLE;
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_BOOTLOADER_STAGE1:
         offender = "bootloader stage-1";
         break;
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_BOOTLOADER_STAGE2:
         offender = "bootloader stage-2";
         break;
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_KERNEL:
         offender = "kernel image";
         break;
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_INITRAMFS:
         offender = "initramfs image";
         break;
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_ROOTFS:
         offender = "rootfs image";
         break;
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_COMMAND_LINE:
         offender = "kernel command-line";
         break;
      default:
         TUX64_UNREACHABLE;
   }

   TUX64_LOG_ERROR_FMT("%s is too large", offender);
   return;
}

static void
tux64_mkrom_exit_result_display(
   const struct Tux64MkromExitResult * self
) {
   switch (self->status) {
      case TUX64_MKROM_EXIT_STATUS_OK:
         break;
      case TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY:
         tux64_mkrom_exit_result_display_out_of_memory(&self->payload.out_of_memory);
         break;
      case TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS:
         tux64_mkrom_exit_result_display_too_many_arguments(&self->payload.too_many_arguments);
         break;
      case TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR:
         /* we do the error reporting in-place to more cleanly solve issues */
         /* with string lifetimes.  thus, no printing needs to be done here. */
         break;
      case TUX64_MKROM_EXIT_STATUS_FS_ERROR:
         tux64_fs_log_result(&self->payload.fs_error.reason);
         break;
      case TUX64_MKROM_EXIT_STATUS_LOAD_ERROR:
         tux64_mkrom_exit_result_display_load_error(&self->payload.load_error);
         break;
      case TUX64_MKROM_EXIT_STATUS_IMAGE_BUILD_ERROR:
         tux64_mkrom_exit_result_display_image_build_error(&self->payload.image_build_error);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static struct Tux64MkromExitResult
tux64_mkrom_main_write_rom_image(
   const struct Tux64MkromImage * image,
   const struct Tux64String * path_output
) {
   struct Tux64MkromExitResult result;
   struct Tux64FsResult save_result;

   TUX64_LOG_INFO_FMT(
      "ROM image is %" PRIu32 " bytes",
      image->bytes
   );

   TUX64_LOG_INFO_FMT(
      "writing ROM image to %.*s",
      path_output->characters,
      path_output->ptr
   );

   save_result = tux64_mkrom_file_save(
      path_output,
      image->data,
      image->bytes
   );
   switch (save_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
         result.payload.out_of_memory.allocation_name = "ROM image writing";
         return result;
      default:
         result.status = TUX64_MKROM_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason = save_result;
         return result;
   }

   /* we finally did it, and with no gotos or disgusting hacks this time! */
   result.status = TUX64_MKROM_EXIT_STATUS_OK;
   return result;
}

static struct Tux64MkromExitResult
tux64_mkrom_main_loaded_data(
   const struct Tux64MkromArgumentsCommandLine * command_line,
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromLoadData * data
) {
   struct Tux64MkromExitResult result;
   struct Tux64MkromImageBuildResult image_build_result;

   TUX64_LOG_INFO("building ROM image");

   image_build_result = tux64_mkrom_image_build(config, data);
   switch (image_build_result.status) {
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OK:
         break;
      case TUX64_MKROM_IMAGE_BUILD_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
         result.payload.out_of_memory.allocation_name = "ROM image building";
         return result;
      default:
         result.status = TUX64_MKROM_EXIT_STATUS_IMAGE_BUILD_ERROR;
         result.payload.image_build_error.reason = image_build_result;
         return result;
   }

   result = tux64_mkrom_main_write_rom_image(
      &image_build_result.payload.ok,
      &command_line->path_output
   );
   tux64_mkrom_image_free(&image_build_result.payload.ok);
   return result;
}

static struct Tux64MkromExitResult
tux64_mkrom_main_parsed_config_file(
   const struct Tux64MkromArgumentsCommandLine * command_line,
   const struct Tux64MkromArgumentsConfigFile * config
) {
   struct Tux64MkromExitResult result;
   struct Tux64MkromLoadResult load_result;

   TUX64_LOG_INFO("parsing boot files from configuration");

   load_result = tux64_mkrom_load_data(config, &command_line->path_prefix);
   switch (load_result.status) {
      case TUX64_MKROM_LOAD_STATUS_OK:
         break;
      case TUX64_MKROM_LOAD_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
         result.payload.out_of_memory.allocation_name = "boot file parsing";
         return result;
      case TUX64_MKROM_LOAD_STATUS_FS_ERROR:
         result.status = TUX64_MKROM_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason = load_result.payload.fs_error.reason;
         return result;
      default:
         result.status = TUX64_MKROM_EXIT_STATUS_LOAD_ERROR;
         result.payload.load_error.reason = load_result;
         return result;
   }

   result = tux64_mkrom_main_loaded_data(
      command_line,
      config,
      &load_result.payload.ok
   );
   tux64_mkrom_load_free_data(&load_result.payload.ok);
   return result;
}

static struct Tux64MkromExitResult
tux64_mkrom_main_loaded_config_data(
   const struct Tux64MkromArgumentsCommandLine * command_line,
   const struct Tux64String * config_data
) {
   struct Tux64MkromExitResult result;
   struct Tux64ArgumentsIterator config_iterator;
   struct Tux64ArgumentsParseResult config_parse_result;
   struct Tux64MkromArgumentsConfigFile config_parsed;

   tux64_arguments_iterator_initialize_config_file(
      &config_iterator,
      &tux64_mkrom_arguments_config_file_iterator_options,
      config_data
   );

   config_parse_result = tux64_mkrom_arguments_config_file_parse(
      &config_iterator,
      &config_parsed
   );
   switch (config_parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         TUX64_UNREACHABLE;
      default:
         tux64_arguments_log_result(&config_parse_result);
         result.status = TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         return result;
   }

   return tux64_mkrom_main_parsed_config_file(command_line, &config_parsed);
}

static struct Tux64MkromExitResult
tux64_mkrom_main_parsed_command_line(
   const struct Tux64MkromArgumentsCommandLine * command_line
) {
   struct Tux64MkromExitResult result;
   struct Tux64FsFileLoadResult config_load_result;
   struct Tux64String config_data;

   TUX64_LOG_INFO_FMT(
      "reading configuration from %.*s",
      command_line->path_config.characters,
      command_line->path_config.ptr
   );

   config_load_result = tux64_mkrom_file_load(&command_line->path_config);
   switch (config_load_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
         result.payload.out_of_memory.allocation_name = "configuration file loading";
         return result;
      default:
         result.status = TUX64_MKROM_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason = tux64_mkrom_file_load_result_to_fs_result(config_load_result);
         return result;
   }

   config_data.ptr         = (const char *)config_load_result.payload.ok.data;
   config_data.characters  = config_load_result.payload.ok.bytes / TUX64_LITERAL_UINT32(sizeof(char));

   result = tux64_mkrom_main_loaded_config_data(command_line, &config_data);
   tux64_fs_file_unload(&config_load_result.payload.ok);
   return result;
}

static struct Tux64MkromExitResult
tux64_mkrom_main(
   Tux64UInt8 argc,
   const char * const * argv
) {
   struct Tux64MkromExitResult result;
   struct Tux64ArgumentsIterator args_iterator;
   struct Tux64ArgumentsParseResult args_parse_result;
   struct Tux64MkromArgumentsCommandLine args_parsed;

   if (argc == TUX64_LITERAL_UINT8(1u)) {
      tux64_mkrom_arguments_command_line_print_menu_help();
      result.status = TUX64_MKROM_EXIT_STATUS_OK;
      return result;
   }

   tux64_arguments_iterator_initialize_command_line(
      &args_iterator,
      argc - TUX64_LITERAL_UINT8(1u),
      &argv[1u]
   );

   args_parse_result = tux64_mkrom_arguments_command_line_parse(
      &args_iterator,
      &args_parsed
   );
   switch (args_parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         result.status = TUX64_MKROM_EXIT_STATUS_OK;
         return result;
      default:
         tux64_arguments_log_result(&args_parse_result);
         result.status = TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         return result;
   }

   return tux64_mkrom_main_parsed_command_line(&args_parsed);
}

int main(int argc, char ** argv) {
   struct Tux64MkromExitResult exit_result;

   if (argc > TUX64_LITERAL_UINT8(TUX64_MKROM_ARGC_MAX)) {
      exit_result.status = TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS;
      exit_result.payload.too_many_arguments.argc_given = argc;
      exit_result.payload.too_many_arguments.argc_max = TUX64_LITERAL_UINT8(TUX64_MKROM_ARGC_MAX);
      goto exit;
   }

   exit_result = tux64_mkrom_main(
      (Tux64UInt8)argc,
      (const char * const *)argv
   );

exit:
   tux64_mkrom_exit_result_display(&exit_result);
   return (int)exit_result.status;
}

