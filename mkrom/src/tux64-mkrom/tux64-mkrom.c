/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/tux64-mkrom.c - Main application entrypoint for      */
/*    tux64-mkrom.                                                            */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"

#include <tux64/log.h>
#include <tux64/memory.h>
#include <tux64/fs.h>
#include <tux64/arguments.h>
#include <tux64/platform-n64/rom.h>
#include "tux64-mkrom/arguments.h"
#include "tux64-mkrom/builder.h"

#include <stdlib.h>
#include <inttypes.h>

#define TUX64_MKROM_ARGC_MAX TUX64_UINT8_MAX

enum Tux64MkromExitStatus {
   TUX64_MKROM_EXIT_STATUS_OK = 0u,
   TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY,
   TUX64_MKROM_EXIT_STATUS_FS_ERROR,
   TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS,
   TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR,
   TUX64_MKROM_EXIT_STATUS_BUILDER_ERROR
};

struct Tux64MkromExitPayloadTooManyArguments {
   int argc_given;
   Tux64UInt8 argc_max;
};

struct Tux64MkromExitPayloadFsError {
   struct Tux64FsResult reason;
};

struct Tux64MkromExitPayloadArgumentsParseError {
   struct Tux64ArgumentsParseResult result;
};

struct Tux64MkromExitPayloadBuilderError {
   struct Tux64MkromBuilderMeasureResult reason;
};

union Tux64MkromExitPayload {
   struct Tux64MkromExitPayloadTooManyArguments too_many_arguments;
   struct Tux64MkromExitPayloadFsError fs_error;
   struct Tux64MkromExitPayloadArgumentsParseError arguments_parse_error;
   struct Tux64MkromExitPayloadBuilderError builder_error;
};

struct Tux64MkromExitResult {
   enum Tux64MkromExitStatus status;
   union Tux64MkromExitPayload payload;
};

static void
tux64_mkrom_exit_result_display_too_many_arguments(
   const struct Tux64MkromExitPayloadTooManyArguments * self
) {
   TUX64_LOG_ERROR_FMT(
      "unable to handle %d arguments at once, maximum is %" PRIu8,
      self->argc_given,
      self->argc_max
   );
   return;
}

static void
tux64_mkrom_exit_result_display_fs_error(
   const struct Tux64MkromExitPayloadFsError * self
) {
   switch (self->reason.status) {
      case TUX64_FS_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_FS_STATUS_NOT_FOUND:
         TUX64_LOG_ERROR("file not found");
         break;
      case TUX64_FS_STATUS_PERMISSION_DENIED:
         TUX64_LOG_ERROR("permission denied");
         break;
      case TUX64_FS_STATUS_NOT_A_FILE:
         TUX64_LOG_ERROR("expected a file, encountered a directory or other non-file object");
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         TUX64_UNREACHABLE;
         break;
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         TUX64_LOG_ERROR_FMT(
            "unknown I/O error (%d)",
            self->reason.payload.unknown_error
         );
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static void
tux64_mkrom_exit_result_display_arguments_parse_error(
   const struct Tux64MkromExitPayloadArgumentsParseError * self
) {
   switch (self->result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         TUX64_UNREACHABLE;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         TUX64_UNREACHABLE;
      case TUX64_ARGUMENTS_PARSE_STATUS_NOT_AN_ARGUMENT:
         TUX64_LOG_ERROR_FMT(
            "\'%.*s\' is not an argument",
            self->result.payload.not_an_argument.argument.characters,
            self->result.payload.not_an_argument.argument.ptr
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_UNKNOWN_IDENTIFIER:
         TUX64_LOG_ERROR_FMT(
            "unknown argument \'%.*s\'",
            self->result.payload.unknown_identifier.identifier.characters,
            self->result.payload.unknown_identifier.identifier.ptr
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_MISSING:
         TUX64_LOG_ERROR_FMT(
            "argument \'%.*s\' expects a parameter, but none was given",
            self->result.payload.parameter_missing.identifier.characters,
            self->result.payload.parameter_missing.identifier
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_UNEXPECTED:
         TUX64_LOG_ERROR_FMT(
            "argument \'%.*s\' doesn't expect parameter \'%.*s\'",
            self->result.payload.parameter_unexpected.identifier.characters,
            self->result.payload.parameter_unexpected.identifier.ptr,
            self->result.payload.parameter_unexpected.parameter.characters,
            self->result.payload.parameter_unexpected.parameter.ptr
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_PARAMETER_INVALID:
         TUX64_LOG_ERROR_FMT(
            "invalid parameter \'%.*s\' for argument \'%.*s\': %.*s",
            self->result.payload.parameter_invalid.parameter.characters,
            self->result.payload.parameter_invalid.parameter.ptr,
            self->result.payload.parameter_invalid.identifier.characters,
            self->result.payload.parameter_invalid.identifier.ptr,
            self->result.payload.parameter_invalid.reason.characters,
            self->result.payload.parameter_invalid.reason.ptr
         );
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_REQUIRED_MISSING:
         TUX64_LOG_ERROR_FMT(
            "missing required argument \'%.*s\'",
            self->result.payload.required_missing.identifier.characters,
            self->result.payload.required_missing.identifier.ptr
         );
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

static const char * const
tux64_mkrom_exit_result_display_builder_error_string_table [TUX64_MKROM_BUILDER_MEASURE_STATUS_FIELD_COUNT - 1u] = {
   "bootloader stage-0 code",
   "bootloader stage-0 CIC data",
   "bootloader stage-1 code",
   "bootloader stage-2 code",
   "bootloader stage-2 BSS data",
   "bootloader stage-3",
   "kernel",
   "initramfs",
   "kernel command-line"
};

static void
tux64_mkrom_exit_result_display_builder_error(
   const struct Tux64MkromExitPayloadBuilderError * self
) {
   const char * label;

   label = tux64_mkrom_exit_result_display_builder_error_string_table[
      (Tux64UInt8)self->reason.status -
      (Tux64UInt8)TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE0
   ];

   TUX64_LOG_ERROR_FMT("%s is too big", label);

   return;
}

static void
tux64_mkrom_exit_result_display(
   const struct Tux64MkromExitResult * self
) {
   switch (self->status) {
      case TUX64_MKROM_EXIT_STATUS_OK:
         break;
      case TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS:
         tux64_mkrom_exit_result_display_too_many_arguments(&self->payload.too_many_arguments);
         break;
      case TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY:
         TUX64_LOG_ERROR("out of memory");
         break;
      case TUX64_MKROM_EXIT_STATUS_FS_ERROR:
         tux64_mkrom_exit_result_display_fs_error(&self->payload.fs_error);
         break;
      case TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR:
         tux64_mkrom_exit_result_display_arguments_parse_error(&self->payload.arguments_parse_error);
         break;
      case TUX64_MKROM_EXIT_STATUS_BUILDER_ERROR:
         tux64_mkrom_exit_result_display_builder_error(&self->payload.builder_error);
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

/* when we want to report errors, we run into a lifetime issue - what if the */
/* string slice comes from data loaded from a file into memory, but then we */
/* need to free that memory?  when do we do that?  i've decided to use a */
/* stack which stores a list of items to clean up after printing error */
/* information. */

#define TUX64_MKROM_EXIT_CLEAN_LIST_ENTRY_TYPE_COUNT 1u
enum Tux64MkromExitCleanListEntryType {
   TUX64_MKROM_EXIT_CLEAN_LIST_ENTRY_TYPE_LOADED_FILE = 0u
};

union Tux64MkromExitCleanListEntryData {
   struct Tux64FsLoadedFile loaded_file;
};

struct Tux64MkromExitCleanListEntry {
   enum Tux64MkromExitCleanListEntryType type;
   union Tux64MkromExitCleanListEntryData data;
};

#define TUX64_MKROM_EXIT_CLEAN_LIST_MAX_ENTRIES\
   1u

struct Tux64MkromExitCleanListEntry
tux64_mkrom_exit_clean_list_table [TUX64_MKROM_EXIT_CLEAN_LIST_MAX_ENTRIES];

Tux64UInt8
tux64_mkrom_exit_clean_list_entries;

static void
tux64_mkrom_exit_clean_list_initialize(void) {
   tux64_mkrom_exit_clean_list_entries = TUX64_LITERAL_UINT8(0u);
   return;
}

typedef void (*Tux64MkromExitCleanListFreeEntryFunction)(
   struct Tux64MkromExitCleanListEntry * entry
);

static void
tux64_mkrom_exit_clean_list_free_entry_loaded_file(
   struct Tux64MkromExitCleanListEntry * entry
) {
   struct Tux64FsLoadedFile * loaded_file;
   
   loaded_file = &entry->data.loaded_file;

   tux64_fs_file_unload(loaded_file);

   return;
}

static const Tux64MkromExitCleanListFreeEntryFunction
tux64_mkrom_exit_clean_list_free_entry_functions [] = {
   tux64_mkrom_exit_clean_list_free_entry_loaded_file
};

static void
tux64_mkrom_exit_clean_list_free_entry(
   struct Tux64MkromExitCleanListEntry * entry
) {
   tux64_mkrom_exit_clean_list_free_entry_functions[(Tux64UInt8)entry->type](entry);
   return;
}

static void
tux64_mkrom_exit_clean_list_free(void) {
   struct Tux64MkromExitCleanListEntry * iter_entry;
   Tux64UInt8 entries_remaining;

   if (tux64_mkrom_exit_clean_list_entries == TUX64_LITERAL_UINT8(0u)) {
      return;
   }

   /* make sure to iterate from top to bottom */
   iter_entry = &tux64_mkrom_exit_clean_list_table[tux64_mkrom_exit_clean_list_entries - TUX64_LITERAL_UINT8(1u)];
   entries_remaining = tux64_mkrom_exit_clean_list_entries;

   do {
      tux64_mkrom_exit_clean_list_free_entry(iter_entry);

      iter_entry--;
      entries_remaining--;
   } while (entries_remaining != TUX64_LITERAL_UINT8(0u));

   return;
}

static void
tux64_mkrom_exit_clean_list_push(
   const struct Tux64MkromExitCleanListEntry * entry
) {
   tux64_mkrom_exit_clean_list_table[tux64_mkrom_exit_clean_list_entries] = *entry;
   tux64_mkrom_exit_clean_list_entries++;
   return;
}

static void
tux64_mkrom_exit_clean_list_push_loaded_file(
   const struct Tux64FsLoadedFile * file
) {
   struct Tux64MkromExitCleanListEntry entry;

   entry.type = TUX64_MKROM_EXIT_CLEAN_LIST_ENTRY_TYPE_LOADED_FILE;
   entry.data.loaded_file = *file;

   tux64_mkrom_exit_clean_list_push(&entry);
   return;
}

/* canonicalizes a path from the command-line into a C-style path string. */
/* the returned pointer must be freed manually, and this returns */
/* TUX64_NULLPTR upon failure. */
static char *
tux64_mkrom_canonicalize_path_command_line(
   const struct Tux64String * path
) {
   Tux64UInt32 characters;
   char * ptr;

   /* includes space for the null-terminator */
   characters = path->characters + TUX64_LITERAL_UINT32(1u);
   ptr = malloc(characters * sizeof(char));
   if (ptr == NULL) {
      return TUX64_NULLPTR;
   }

   tux64_memory_copy(ptr, path->ptr, path->characters * TUX64_LITERAL_UINT32(sizeof(char)));
   ptr[characters - TUX64_LITERAL_UINT32(1u)] = '\0';

   return ptr;
}

/* canonicalizes a path from the config file into a C-style path string. */
/* the returned pointer must be freed manually, and this returns */
/* TUX64_NULLPTR upon failure. */
static char *
tux64_mkrom_canonicalize_path_config_file(
   const struct Tux64String * prefix,
   const struct Tux64String * path
) {
   Tux64UInt32 characters;
   char * ptr;

   /* includes space for the null-terminator */
   characters = prefix->characters + path->characters + TUX64_LITERAL_UINT32(1u);
   ptr = malloc(characters * sizeof(char));
   if (ptr == NULL) {
      return TUX64_NULLPTR;
   }

   tux64_memory_copy(
      ptr,
      prefix->ptr,
      prefix->characters * TUX64_LITERAL_UINT32(sizeof(char))
   );
   tux64_memory_copy(
      ptr + prefix->characters,
      path->ptr, path->characters * TUX64_LITERAL_UINT32(sizeof(char))
   );
   ptr[characters - TUX64_LITERAL_UINT32(1u)] = '\0';

   return ptr;
}

static struct Tux64MkromExitResult
tux64_mkrom_load_file(
   const char * path_canonical,
   const char * name,
   struct Tux64FsLoadedFile * output
) {
   struct Tux64FsFileLoadResult load_result;
   struct Tux64MkromExitResult result;

   TUX64_LOG_INFO_FMT("loading %s from %s", name, path_canonical);

   load_result = tux64_fs_file_load(path_canonical);

   switch (load_result.status) {
      case TUX64_FS_STATUS_OK:
         result.status = TUX64_MKROM_EXIT_STATUS_OK;
         *output = load_result.payload.ok;
         break;

      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_MKROM_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = load_result.status;
         result.payload.fs_error.reason.payload = load_result.payload.err;
         break;

      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
         break;

      default:
         TUX64_UNREACHABLE;
   }

   return result;
}

static struct Tux64MkromExitResult
tux64_mkrom_save_file(
   const char * path_canonical,
   const char * name,
   const struct Tux64FsLoadedFile * file
) {
   struct Tux64FsResult save_result;
   struct Tux64MkromExitResult result;

   TUX64_LOG_INFO_FMT("saving %s to %s", name, path_canonical);

   save_result = tux64_fs_file_save(path_canonical, file);

   switch (save_result.status) {
      case TUX64_FS_STATUS_OK:
         result.status = TUX64_MKROM_EXIT_STATUS_OK;
         break;

      case TUX64_FS_STATUS_NOT_FOUND:
      case TUX64_FS_STATUS_PERMISSION_DENIED:
      case TUX64_FS_STATUS_NOT_A_FILE:
      case TUX64_FS_STATUS_UNKNOWN_ERROR:
         result.status = TUX64_MKROM_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason = save_result;
         break;

      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
         break;

      default:
         TUX64_UNREACHABLE;
   }

   return result;
}

static struct Tux64MkromExitResult
tux64_mkrom_load_file_command_line(
   const struct Tux64String * path,
   const char * name,
   struct Tux64FsLoadedFile * output
) {
   struct Tux64MkromExitResult result;
   char * path_canonical;

   path_canonical = tux64_mkrom_canonicalize_path_command_line(path);
   if (path_canonical == TUX64_NULLPTR) {
      result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   result = tux64_mkrom_load_file(path_canonical, name, output);
   free(path_canonical);
   return result;
}

static struct Tux64MkromExitResult
tux64_mkrom_save_file_command_line(
   const struct Tux64String * path,
   const char * name,
   const struct Tux64FsLoadedFile * file
) {
   struct Tux64MkromExitResult result;
   char * path_canonical;

   path_canonical = tux64_mkrom_canonicalize_path_command_line(path);
   if (path_canonical == TUX64_NULLPTR) {
      result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   result = tux64_mkrom_save_file(path_canonical, name, file);
   free(path_canonical);
   return result;
}

static struct Tux64MkromExitResult
tux64_mkrom_load_file_config_file(
   const struct Tux64String * prefix,
   const struct Tux64String * path,
   const char * name,
   struct Tux64FsLoadedFile * output
) {
   struct Tux64MkromExitResult result;
   char * path_canonical;

   path_canonical = tux64_mkrom_canonicalize_path_config_file(prefix, path);
   if (path_canonical == TUX64_NULLPTR) {
      result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   result = tux64_mkrom_load_file(path_canonical, name, output);
   free(path_canonical);
   return result;
}

struct Tux64MkromInputFilesBootloader {
   struct Tux64FsLoadedFile stage0;
   struct Tux64FsLoadedFile stage0_cic;
   struct Tux64FsLoadedFile stage1;
   struct Tux64FsLoadedFile stage2;
   struct Tux64FsLoadedFile stage2_bss;
   struct Tux64FsLoadedFile stage3;
};

struct Tux64MkromInputFiles {
   struct Tux64MkromInputFilesBootloader bootloader;
   struct Tux64FsLoadedFile kernel;
   struct Tux64FsLoadedFile initramfs;
};

struct Tux64MkromInput {
   struct Tux64MkromInputFiles files;
   const struct Tux64PlatformN64RomHeader * rom_header;
   struct Tux64String kernel_command_line;
   struct Tux64String path_output;
};

static struct Tux64MkromExitResult
tux64_mkrom_run_parsed_input(
   const struct Tux64MkromInput * input
) {
   struct Tux64MkromExitResult result;
   struct Tux64MkromBuilderInput builder_input;
   struct Tux64MkromBuilderMeasureResult measure_result;
   Tux64UInt8 * rom_file_data;
   struct Tux64FsLoadedFile rom_file;

   /* we do this to restrict mutable pointers and also work around previous */
   /* bad code, and I don't feel like rewriting it because it's boring. */
   builder_input.files.bootloader.stage0.data = input->files.bootloader.stage0.data;
   builder_input.files.bootloader.stage0.bytes = input->files.bootloader.stage0.bytes;
   builder_input.files.bootloader.stage0_cic.data = input->files.bootloader.stage0_cic.data;
   builder_input.files.bootloader.stage0_cic.bytes = input->files.bootloader.stage0_cic.bytes;
   builder_input.files.bootloader.stage1.data = input->files.bootloader.stage1.data;
   builder_input.files.bootloader.stage1.bytes = input->files.bootloader.stage1.bytes;
   builder_input.files.bootloader.stage2.data = input->files.bootloader.stage2.data;
   builder_input.files.bootloader.stage2.bytes = input->files.bootloader.stage2.bytes;
   builder_input.files.bootloader.stage2_bss.data = input->files.bootloader.stage2_bss.data;
   builder_input.files.bootloader.stage2_bss.bytes = input->files.bootloader.stage2_bss.bytes;
   builder_input.files.bootloader.stage3.data = input->files.bootloader.stage3.data;
   builder_input.files.bootloader.stage3.bytes = input->files.bootloader.stage3.bytes;
   builder_input.files.kernel.data = input->files.kernel.data;
   builder_input.files.kernel.bytes = input->files.kernel.bytes;
   builder_input.files.initramfs.data = input->files.initramfs.data;
   builder_input.files.initramfs.bytes = input->files.initramfs.bytes;
   tux64_memory_copy(&builder_input.rom_header, input->rom_header, TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformN64RomHeader)));
   builder_input.kernel_command_line = input->kernel_command_line;

   TUX64_LOG_INFO("verifying input files and calculating ROM length");

   measure_result = tux64_mkrom_builder_measure_and_verify(&builder_input);
   if (measure_result.status != TUX64_MKROM_BUILDER_MEASURE_STATUS_OK) {
      result.status = TUX64_MKROM_EXIT_STATUS_BUILDER_ERROR;
      result.payload.builder_error.reason = measure_result;
      return result;
   }

   TUX64_LOG_INFO_FMT("ROM will be %" PRIu32 " bytes", measure_result.payload.ok.rom_bytes);

   rom_file_data = malloc(measure_result.payload.ok.rom_bytes * sizeof(Tux64UInt8));
   if (rom_file_data == NULL) {
      result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
      return result;
   }

   rom_file.data = rom_file_data;
   rom_file.bytes = measure_result.payload.ok.rom_bytes;

   TUX64_LOG_INFO("constructing ROM image");

   tux64_mkrom_builder_construct(
      &builder_input,
      &measure_result.payload.ok,
      rom_file.data
   );

   result = tux64_mkrom_save_file_command_line(
      &input->path_output,
      "ROM image",
      &rom_file
   );
   tux64_fs_file_unload(&rom_file);
   return result;
}

static struct Tux64MkromExitResult
tux64_mkrom_run_parsed_cmdline(
   const struct Tux64MkromArgumentsCommandLine * cmdline
) {
   struct Tux64MkromExitResult result;
   struct Tux64FsLoadedFile config_file;
   struct Tux64String config_file_string;
   struct Tux64ArgumentsIterator config_file_arguments_iterator;
   struct Tux64ArgumentsParseResult config_file_parse_result;
   struct Tux64MkromArgumentsConfigFile config_file_parsed;
   struct Tux64MkromInput input;
   char * kernel_command_line_ptr;
   Tux64Boolean config_file_loaded;

   /* attempt to load the config file into memory */
   result = tux64_mkrom_load_file_command_line(
      &cmdline->path_config,
      "config file",
      &config_file
   );
   if (result.status != TUX64_MKROM_EXIT_STATUS_OK) {
      return result;
   }
   config_file_loaded = TUX64_BOOLEAN_TRUE;

   /* convert the raw data into a string */
   config_file_string.ptr = (const char *)config_file.data;
   config_file_string.characters =
      config_file.bytes /
      TUX64_LITERAL_UINT32(sizeof(char));

   /* set up the arguments iterator for the config file */
   tux64_arguments_iterator_initialize_config_file(
      &config_file_arguments_iterator,
      &tux64_mkrom_arguments_config_file_iterator_options,
      &config_file_string
   );

   /* attempt to parse the config file */
   /* note that we must wait until the end of the function to unload, as */
   /* we have borrowed data passed around from it, and copying sucks. */
   config_file_parse_result = tux64_mkrom_arguments_config_file_parse(
      &config_file_arguments_iterator,
      &config_file_parsed
   );
   switch (config_file_parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         TUX64_UNREACHABLE;
      default:
         /* save freeing until the end of the program due to lifetimes */
         tux64_mkrom_exit_clean_list_push_loaded_file(&config_file);
         result.status = TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         result.payload.arguments_parse_error.result = config_file_parse_result;
         return result;
   }

   /* load all the files into memory */
   result = tux64_mkrom_load_file_config_file(
      &cmdline->path_prefix,
      &config_file_parsed.path_bootloader_stage0,
      "bootloader stage-0 code",
      &input.files.bootloader.stage0
   );
   if (result.status != TUX64_MKROM_EXIT_STATUS_OK) {
      goto load_err_exit0;
   }
   result = tux64_mkrom_load_file_config_file(
      &cmdline->path_prefix,
      &config_file_parsed.path_bootloader_stage0_cic,
      "bootloader stage-0 CIC data",
      &input.files.bootloader.stage0_cic
   );
   if (result.status != TUX64_MKROM_EXIT_STATUS_OK) {
      goto load_err_exit1;
   }
   result = tux64_mkrom_load_file_config_file(
      &cmdline->path_prefix,
      &config_file_parsed.path_bootloader_stage1,
      "bootloader stage-1 code",
      &input.files.bootloader.stage1
   );
   if (result.status != TUX64_MKROM_EXIT_STATUS_OK) {
      goto load_err_exit2;
   }
   result = tux64_mkrom_load_file_config_file(
      &cmdline->path_prefix,
      &config_file_parsed.path_bootloader_stage2,
      "bootloader stage-2 code",
      &input.files.bootloader.stage2
   );
   if (result.status != TUX64_MKROM_EXIT_STATUS_OK) {
      goto load_err_exit3;
   }
   result = tux64_mkrom_load_file_config_file(
      &cmdline->path_prefix,
      &config_file_parsed.path_bootloader_stage2_bss,
      "bootloader stage-2 BSS data",
      &input.files.bootloader.stage2_bss
   );
   if (result.status != TUX64_MKROM_EXIT_STATUS_OK) {
      goto load_err_exit4;
   }
   result = tux64_mkrom_load_file_config_file(
      &cmdline->path_prefix,
      &config_file_parsed.path_bootloader_stage3,
      "bootloader stage-3 code",
      &input.files.bootloader.stage3
   );
   if (result.status != TUX64_MKROM_EXIT_STATUS_OK) {
      goto load_err_exit5;
   }
   result = tux64_mkrom_load_file_config_file(
      &cmdline->path_prefix,
      &config_file_parsed.path_kernel,
      "kernel image",
      &input.files.kernel
   );
   if (result.status != TUX64_MKROM_EXIT_STATUS_OK) {
      goto load_err_exit6;
   }
   result = tux64_mkrom_load_file_config_file(
      &cmdline->path_prefix,
      &config_file_parsed.path_initramfs,
      "initramfs image",
      &input.files.initramfs
   );
   if (result.status != TUX64_MKROM_EXIT_STATUS_OK) {
      goto load_err_exit7;
   }

   /* create an owned copy of the kernel command-line */
   kernel_command_line_ptr = malloc(config_file_parsed.command_line.characters * sizeof(char));
   if (kernel_command_line_ptr == NULL) {
      result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
      goto load_err_exit8;
   }
   tux64_memory_copy(
      kernel_command_line_ptr,
      config_file_parsed.command_line.ptr,
      config_file_parsed.command_line.characters * TUX64_LITERAL_UINT32(sizeof(char))
   );

   /* initialize the rest of the fields for the input */
   input.rom_header = &config_file_parsed.rom_header;
   input.kernel_command_line.ptr = kernel_command_line_ptr;
   input.kernel_command_line.characters = config_file_parsed.command_line.characters;
   input.path_output = cmdline->path_output;

   /* we can now safely free the config file since all data is owned */
   tux64_fs_file_unload(&config_file);
   config_file_loaded = TUX64_BOOLEAN_FALSE;

   /* and now we commence to the actual program, hoo-ray... */
   result = tux64_mkrom_run_parsed_input(&input);

   /* ...but don't forget to clean up after ourselves! */
   free(kernel_command_line_ptr);
load_err_exit8:
   tux64_fs_file_unload(&input.files.initramfs);
load_err_exit7:
   tux64_fs_file_unload(&input.files.kernel);
load_err_exit6:
   tux64_fs_file_unload(&input.files.bootloader.stage3);
load_err_exit5:
   tux64_fs_file_unload(&input.files.bootloader.stage2_bss);
load_err_exit4:
   tux64_fs_file_unload(&input.files.bootloader.stage2);
load_err_exit3:
   tux64_fs_file_unload(&input.files.bootloader.stage1);
load_err_exit2:
   tux64_fs_file_unload(&input.files.bootloader.stage0_cic);
load_err_exit1:
   tux64_fs_file_unload(&input.files.bootloader.stage0);
load_err_exit0:
   if (config_file_loaded == TUX64_BOOLEAN_TRUE) {
      tux64_fs_file_unload(&config_file);
   }
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
   struct Tux64MkromArgumentsCommandLine args_cmdline;

   /* if the user is completely clueless, show them the help menu when no */
   /* arguments are present */
   if (argc == TUX64_LITERAL_UINT8(1u)) {
      tux64_mkrom_arguments_command_line_print_menu_help();
      result.status = TUX64_MKROM_EXIT_STATUS_OK;
      return result;
   }

   /* makes sure to skip over argv[0] */
   tux64_arguments_iterator_initialize_command_line(
      &args_iterator,
      argc - TUX64_LITERAL_UINT8(1u),
      &argv[1]
   );

   /* parse command-line arguments */
   args_parse_result = tux64_mkrom_arguments_command_line_parse(
      &args_iterator,
      &args_cmdline
   );
   switch (args_parse_result.status) {
      case TUX64_ARGUMENTS_PARSE_STATUS_OK:
         break;
      case TUX64_ARGUMENTS_PARSE_STATUS_EXIT:
         result.status = TUX64_MKROM_EXIT_STATUS_OK;
         return result;
      default:
         result.status = TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         result.payload.arguments_parse_error.result = args_parse_result;
         return result;
   }

   return tux64_mkrom_run_parsed_cmdline(&args_cmdline);
}

int main(int argc, char ** argv) {
   struct Tux64MkromExitResult exit_result;

   tux64_mkrom_exit_clean_list_initialize();

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
   tux64_mkrom_exit_clean_list_free();
   return (int)exit_result.status;
}

