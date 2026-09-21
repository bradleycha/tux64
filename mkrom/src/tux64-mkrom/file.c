/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/tux64-mkrom.c - Main application entrypoint for      */
/*    tux64-mkrom.                                                            */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/file.h"

#include <tux64/fs.h>
#include <tux64/memory.h>

#include <stdlib.h>

/* TODO: we may want to move these into tux64-lib.  these are pretty generic  */
/* functions, and file saving is a bit hacky.  the only thing which is        */
/* appropriate to be mkrom-specific is loading prefixed files.                */

static char *
tux64_mkrom_file_format_path_to_cstr(
   const struct Tux64String * path
) {
   char * path_cstr;

   path_cstr = (char *)malloc((path->characters + 1u) * sizeof(char));
   if (path_cstr == NULL) {
      return NULL;
   }

   tux64_memory_copy(
      path_cstr,
      path->ptr,
      path->characters * TUX64_LITERAL_UINT32(sizeof(char))
   );
   path_cstr[path->characters] = '\0';

   return path_cstr;
}

struct Tux64FsFileLoadResult
tux64_mkrom_file_load(
   const struct Tux64String * path
) {
   struct Tux64FsFileLoadResult result;
   char * path_cstr;

   path_cstr = tux64_mkrom_file_format_path_to_cstr(path);
   if (path_cstr == NULL) {
      result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
      return result;
   }

   result = tux64_fs_file_load(path_cstr);
   free(path_cstr);
   return result;
}

struct Tux64FsFileLoadResult
tux64_mkrom_file_load_from_prefix(
   const struct Tux64String * path,
   const struct Tux64String * prefix
) {
   struct Tux64FsFileLoadResult result;
   Tux64UInt32 path_length;
   char * path_cstr;

   path_length = prefix->characters + path->characters;

   path_cstr = (char *)malloc((path_length + 1u) * sizeof(char));
   if (path_cstr == NULL) {
      result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
      return result;
   }

   tux64_memory_copy(
      &path_cstr[0u],
      prefix->ptr,
      prefix->characters * TUX64_LITERAL_UINT32(sizeof(char))
   );
   tux64_memory_copy(
      &path_cstr[prefix->characters],
      path->ptr,
      path->characters * TUX64_LITERAL_UINT32(sizeof(char))
   );
   path_cstr[path_length] = '\0';

   result = tux64_fs_file_load(path_cstr);
   free(path_cstr);
   return result;
}

struct Tux64FsResult
tux64_mkrom_file_save(
   const struct Tux64String * path,
   const void * data,
   Tux64UInt32 bytes
) {
   struct Tux64FsResult result;
   char * path_cstr;
   struct Tux64FsLoadedFile file;

   path_cstr = tux64_mkrom_file_format_path_to_cstr(path);
   if (path_cstr == NULL) {
      result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
      return result;
   }

   file.data   = (Tux64UInt8 *)data;
   file.bytes  = bytes;

   result = tux64_fs_file_save(path_cstr, &file);
   free(path_cstr);
   return result;
}

struct Tux64FsResult
tux64_mkrom_file_load_result_to_fs_result(
   struct Tux64FsFileLoadResult file_load_result
) {
   struct Tux64FsResult fs_result;

   fs_result.status = file_load_result.status;

   /* needed to prevent invoking undefined behavior by copying uninitialized */
   /* data. */
   if (fs_result.status != TUX64_FS_STATUS_UNKNOWN_ERROR) {
      fs_result.payload = file_load_result.payload.err;
   }

   return fs_result;
}

