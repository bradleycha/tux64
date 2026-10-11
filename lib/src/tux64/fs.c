/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/fs.c - Implementations for filesystem functions.             */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/fs.h"

#if _TUX64_FS_ENABLE
/*----------------------------------------------------------------------------*/

#include "tux64/log.h"
#include "tux64/string.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#if TUX64_IMPLEMENTATION_CORE
/*----------------------------------------------------------------------------*/

struct Tux64FsResult
tux64_fs_file_save_cstr(
   const char * path,
   const struct Tux64FsLoadedFile * file
) {
   return tux64_fs_file_save_data_cstr(path, file->data, file->bytes);
}

struct Tux64FsResult
tux64_fs_file_save(
   const struct Tux64String * path,
   const struct Tux64FsLoadedFile * file
) {
   return tux64_fs_file_save_data(path, file->data, file->bytes);
}

void
tux64_fs_file_unload(
   struct Tux64FsLoadedFile * file
) {
   free(file->data);
   return;
}

struct Tux64FsResult
tux64_fs_file_load_result_to_fs_result(
   struct Tux64FsFileLoadResult file_load_result
) {
   struct Tux64FsResult fs_result;

   fs_result.status = file_load_result.status;

   /* needed to prevent invoking undefined behavior by copying uninitialized */
   /* data.  ideally, the compiler should optimize this out because it */
   /* doesn't matter, but i haven't bothered to check this. */
   if (fs_result.status == TUX64_FS_STATUS_UNKNOWN_ERROR) {
      fs_result.payload.unknown_error = file_load_result.payload.err.unknown_error;
   }

   return fs_result;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_IMPLEMENTATION_CORE */

#if TUX64_IMPLEMENTATION_LIB
/*----------------------------------------------------------------------------*/

static struct Tux64FsResult
tux64_fs_errno_to_fs_result(
   int err
) {
   struct Tux64FsResult result;

   /* "errno" - i can't believe it's not local! */
   /* ... */
   /* seriously, what the FUCK were the C standard developers thinking with */
   /* this bullshit interface? just return an enum for fucks sake...but NO! */
   /* instead we get this fucked-up shit where we return NULL then set a */
   /* GLOBAL VARIABLE that *LOOKS* like a local variable... */
   /* also, the entire stdio header is a horrible abortion of an API. */

   switch (err) {
      case 0:
         /* we should only call this function when an error ocurred.  if */
         /* errno is set to zero when we failed, then file a bug-report to */
         /* the assholes who wrote this horrendous interface. */
         TUX64_UNREACHABLE;

      case ENOENT:
         result.status = TUX64_FS_STATUS_NOT_FOUND;
         break;

      case EACCES:
         /* these people are so stupid, they can't even spell their OWN */
         /* damn shiterface correctly! */
         result.status = TUX64_FS_STATUS_PERMISSION_DENIED;
         break;

      case EISDIR:
         result.status = TUX64_FS_STATUS_NOT_A_FILE;
         break;

      case ENOMEM:
         /* hurr-durr, i'm a fuckwit who eats sand and drinks piss.  i also */
         /* use fallible memory allocators for I/O operations which have NO */
         /* reason to allocate memory. */
         result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
         break;

      case EROFS:
         result.status = TUX64_FS_STATUS_PERMISSION_DENIED;
         break;

      case ENOSPC:
         result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
         break;

      default:
         /* do we document what can be returned? do we use an enum? NO! libc */
         /* just returns some random shit and says "best of luck, fucker! */
         /* good luck parsing this crap!" */
         result.status = TUX64_FS_STATUS_UNKNOWN_ERROR;
         result.payload.unknown_error.code = err;
         break;
   }

   return result;
}

static struct Tux64FsFileLoadResult
tux64_fs_errno_to_fs_file_load_result(
   int err
) {
   struct Tux64FsFileLoadResult result;
   struct Tux64FsResult fs_result;

   fs_result = tux64_fs_errno_to_fs_result(err);

   result.status = fs_result.status;

   /* this needs to be checked, otherwise we end up copying an uninitialized */
   /* variable, invoking undefined behavior.  it doesn't matter that calling */
   /* code (shouldn't) access this field, because we're accessing it here. */
   if (fs_result.status == TUX64_FS_STATUS_UNKNOWN_ERROR) {
      result.payload.err = fs_result.payload;
   }

   return result;
}

static struct Tux64FsFileLoadResult
tux64_fs_file_load_allocated(
   FILE * file,
   Tux64UInt8 * read_buffer,
   Tux64UInt32 read_bytes_total
) {
   struct Tux64FsFileLoadResult result;
   Tux64UInt8 * iter_read_buffer;
   Tux64UInt32 read_bytes_remaining;
   Tux64UInt32 read_bytes;

   iter_read_buffer = read_buffer;
   read_bytes_remaining = read_bytes_total;

   while (read_bytes_remaining != TUX64_LITERAL_UINT32(0u)) {
      read_bytes = (Tux64UInt32)fread(
         iter_read_buffer,
         (size_t)sizeof(Tux64UInt8),
         read_bytes_remaining,
         file
      );
      if (ferror(file) != 0) {
         result = tux64_fs_errno_to_fs_file_load_result(errno);
         return result;
      }

      iter_read_buffer += read_bytes;
      read_bytes_remaining -= read_bytes;
   }

   result.status = TUX64_FS_STATUS_OK;
   result.payload.ok.data = read_buffer;
   result.payload.ok.bytes = read_bytes_total;
   return result;
}

static struct Tux64FsFileLoadResult
tux64_fs_file_load_open(
   FILE * file
) {
   struct Tux64FsFileLoadResult result;
   long read_bytes;
   Tux64UInt8 * read_buffer;

   /* this ridiculous code is required just to get the number of bytes in a */
   /* file. */
   if (fseek(file, 0, SEEK_END) != 0) {
      result = tux64_fs_errno_to_fs_file_load_result(errno);
      return result;
   }
   read_bytes = ftell(file);
   if (read_bytes < 0) {
      result = tux64_fs_errno_to_fs_file_load_result(errno);
      return result;
   }
   rewind(file);

   /* now we check for differing types to overflow since everything is */
   /* incompatible and of non-standard size */
#if TUX64_SIZEOF_LONG > 4
   if ((unsigned long)read_bytes > TUX64_UINT32_MAX) {
      result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
      return result;
   }
#endif /* TUX64_SIZEOF_LONG > 4 */

   /* we check again just for good measure */
   if ((unsigned long)read_bytes > SIZE_MAX) {
      result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
      return result;
   }

   /* and now we allocate some bullshit for the file contents */
   read_buffer = (Tux64UInt8 *)malloc((size_t)read_bytes);
   if (read_buffer == NULL) {
      result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
      return result;
   }

   /* and we continue to actually loading the file, freeing the bullshit */
   /* if we encounter an error.  so basically we implement errdefer from zig. */
   result = tux64_fs_file_load_allocated(
      file,
      read_buffer,
      (Tux64UInt32)read_bytes
   );
   if (result.status != TUX64_FS_STATUS_OK) {
      free(read_buffer);
   }
   return result;
}

struct Tux64FsFileLoadResult
tux64_fs_file_load_cstr(
   const char * path
) {
   struct Tux64FsFileLoadResult result;
   FILE * f;

   f = fopen(path, "r");
   if (f == NULL) {
      result = tux64_fs_errno_to_fs_file_load_result(errno);
      return result;
   }

   result = tux64_fs_file_load_open(f);
   (void)fclose(f);
   return result;
}

struct Tux64FsFileLoadResult
tux64_fs_file_load(
   const struct Tux64String * path
) {
   struct Tux64FsFileLoadResult result;
   char * path_cstr;

   path_cstr = tux64_string_allocate_and_format_cstr(path);
   if (path_cstr == TUX64_NULLPTR) {
      result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
      return result;
   }

   result = tux64_fs_file_load_cstr(path_cstr);
   free(path_cstr);
   return result;
}

static struct Tux64FsResult
tux64_fs_file_save_open(
   FILE * file_dest,
   const Tux64UInt8 data[],
   Tux64UInt32 bytes
) {
   struct Tux64FsResult result;
   const Tux64UInt8 * iter_write_buffer;
   Tux64UInt32 write_bytes_remaining;
   Tux64UInt32 written_bytes;

   iter_write_buffer = data;
   write_bytes_remaining = bytes;

   while (write_bytes_remaining != TUX64_LITERAL_UINT32(0u)) {
      written_bytes = fwrite(
         iter_write_buffer,
         (size_t)sizeof(Tux64UInt8),
         write_bytes_remaining,
         file_dest
      );
      if (ferror(file_dest) != 0) {
         result = tux64_fs_errno_to_fs_result(errno);
         return result;
      }

      iter_write_buffer += written_bytes;
      write_bytes_remaining -= written_bytes;
   }

   result.status = TUX64_FS_STATUS_OK;
   return result;
}

struct Tux64FsResult
tux64_fs_file_save_data_cstr(
   const char * path,
   const Tux64UInt8 data[],
   Tux64UInt32 bytes
) {
   struct Tux64FsResult result;
   FILE * file_dest;

   file_dest = fopen(path, "w");
   if (file_dest != NULL) {
      result = tux64_fs_file_save_open(file_dest, data, bytes);
      (void)fclose(file_dest);
      return result;
   }

   result = tux64_fs_errno_to_fs_result(errno);
   return result;
}

struct Tux64FsResult
tux64_fs_file_save_data(
   const struct Tux64String * path,
   const Tux64UInt8 data[],
   Tux64UInt32 bytes
) {
   struct Tux64FsResult result;
   char * path_cstr;

   path_cstr = tux64_string_allocate_and_format_cstr(path);
   if (path_cstr == TUX64_NULLPTR) {
      result.status = TUX64_FS_STATUS_OUT_OF_MEMORY;
      return result;
   }

   result = tux64_fs_file_save_data_cstr(path_cstr, data, bytes);
   free(path_cstr);
   return result;
}

void
tux64_fs_log_result(
   const struct Tux64FsResult * result
) {
   switch (result->status) {
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
            result->payload.unknown_error
         );
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_IMPLEMENTATION_LIB */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_FS_ENABLE */

