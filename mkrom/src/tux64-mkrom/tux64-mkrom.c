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
#include "tux64-mkrom/arguments.h"

#include <stdlib.h>
#include <inttypes.h>

#define TUX64_MKROM_ARGC_MAX TUX64_UINT8_MAX

enum Tux64MkromExitStatus {
   TUX64_MKROM_EXIT_STATUS_OK = 0u,
   TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY,
   TUX64_MKROM_EXIT_STATUS_FS_ERROR,
   TUX64_MKROM_EXIT_STATUS_TOO_MANY_ARGUMENTS,
   TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR
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

union Tux64MkromExitPayload {
   struct Tux64MkromExitPayloadTooManyArguments too_many_arguments;
   struct Tux64MkromExitPayloadFsError fs_error;
   struct Tux64MkromExitPayloadArgumentsParseError arguments_parse_error;
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

/* converts a prefix/path string pair into a C-string which can be used with */
/* file operations.  return TUX64_NULLPTR when memory allocation fails. this */
/* must be freed manually by the caller. */
static char *
tux64_mkrom_canonicalize_path(
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

   tux64_memory_copy(ptr, prefix->ptr, prefix->characters);
   tux64_memory_copy(ptr + prefix->characters, path->ptr, path->characters);
   ptr[characters - TUX64_LITERAL_UINT32(1u)] = '\0';

   return ptr;
}

static struct Tux64MkromExitResult
tux64_mkrom_run_parsed_cmdline(
   const struct Tux64String * path_prefix,
   const char * path_canonical_config,
   const char * path_canonical_output
) {
   struct Tux64MkromExitResult result;
   struct Tux64FsFileLoadResult config_file_load_result;
   struct Tux64String config_file_string;
   struct Tux64ArgumentsIterator config_file_arguments_iterator;
   struct Tux64ArgumentsParseResult config_file_parse_result;
   struct Tux64MkromArgumentsConfigFile config_file_parsed;

   /* attempt to load the config file into memory */
   TUX64_LOG_INFO_FMT("loading config file from %s", path_canonical_config);
   config_file_load_result = tux64_fs_file_load(path_canonical_config);
   switch (config_file_load_result.status) {
      case TUX64_FS_STATUS_OK:
         break;
      case TUX64_FS_STATUS_OUT_OF_MEMORY:
         result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
         return result;
      default:
         result.status = TUX64_MKROM_EXIT_STATUS_FS_ERROR;
         result.payload.fs_error.reason.status = config_file_load_result.status;
         result.payload.fs_error.reason.payload = config_file_load_result.payload.err;
         return result;
   }

   /* convert the raw data into a string */
   config_file_string.ptr = (const char *)config_file_load_result.payload.ok.data;
   config_file_string.characters =
      config_file_load_result.payload.ok.bytes /
      TUX64_LITERAL_UINT32(sizeof(char));

   /* set up the arguments iterator for the config file */
   tux64_arguments_iterator_initialize_config_file(
      &config_file_arguments_iterator,
      &tux64_mkrom_arguments_config_file_iterator_options,
      &config_file_string
   );

   /* attempt to parse the config file */
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
         tux64_mkrom_exit_clean_list_push_loaded_file(&config_file_load_result.payload.ok);
         result.status = TUX64_MKROM_EXIT_STATUS_ARGUMENTS_PARSE_ERROR;
         result.payload.arguments_parse_error.result = config_file_parse_result;
         return result;
   }

   /* TODO: this is what we have to do now: */
   /* 1. canonicalize all paths in the config file */
   /* 2. make owned copies of all other borrowed data in the config file */
   /* 3. free the config file string from memory */
   /* 4. combine the command-line arguments and config arguments into a unified */
   /*    'options' struct */
   /* 5. implement the actual program :) */
   TUX64_LOG_INFO("parsed the configuration file!");
   (void)path_prefix;
   (void)path_canonical_output;
   tux64_fs_file_unload(&config_file_load_result.payload.ok);
   result.status = TUX64_MKROM_EXIT_STATUS_OK;
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
   char * path_config;
   char * path_output;

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

   /* canonicalize the config path and output path */
   path_config = tux64_mkrom_canonicalize_path(
      &args_cmdline.path_prefix,
      &args_cmdline.path_config
   );
   if (path_config == TUX64_NULLPTR) {
      result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
      goto exit0;
   }
   path_output = tux64_mkrom_canonicalize_path(
      &args_cmdline.path_prefix,
      &args_cmdline.path_output
   );
   if (path_output == TUX64_NULLPTR) {
      result.status = TUX64_MKROM_EXIT_STATUS_OUT_OF_MEMORY;
      goto exit1;
   }

   result = tux64_mkrom_run_parsed_cmdline(
      &args_cmdline.path_prefix,
      path_config,
      path_output
   );
   free(path_output);
exit1:
   free(path_config);
exit0:
   return result;
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

