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
#include <tux64/string.h>

#include <stdlib.h>

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

   result = tux64_fs_file_load_cstr(path_cstr);
   free(path_cstr);
   return result;
}

