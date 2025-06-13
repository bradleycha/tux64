/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/fs.h - Header for filesystem functions.                      */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_FS_H
#define _TUX64_FS_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

#define _TUX64_FS_ENABLE\
   (TUX64_HAVE_STDLIB_H && TUX64_HAVE_STDIO_H && TUX64_HAVE_ERRNO_H)

#if _TUX64_FS_ENABLE
/*----------------------------------------------------------------------------*/

enum Tux64FsStatus {
   TUX64_FS_STATUS_OK,
   TUX64_FS_STATUS_NOT_FOUND,
   TUX64_FS_STATUS_PERMISSION_DENIED,
   TUX64_FS_STATUS_NOT_A_FILE,
   TUX64_FS_STATUS_OUT_OF_MEMORY,
   TUX64_FS_STATUS_UNKNOWN_ERROR
};

struct Tux64FsPayloadUnknownError {
   int code;
};

union Tux64FsPayload {
   struct Tux64FsPayloadUnknownError unknown_error;
};

struct Tux64FsResult {
   enum Tux64FsStatus status;
   union Tux64FsPayload payload;
};

struct Tux64FsLoadedFile {
   Tux64UInt8 * data;
   Tux64UInt32 bytes;
};

union Tux64FsFileLoadPayload {
   struct Tux64FsLoadedFile ok;
   union Tux64FsPayload err;
};

struct Tux64FsFileLoadResult {
   enum Tux64FsStatus status;
   union Tux64FsFileLoadPayload payload;
};

/*----------------------------------------------------------------------------*/
/* Attempts to load a file into memory from the given path.  The file must be */
/* freed using tux64_fs_file_unload() once no longer needed.                  */
/*----------------------------------------------------------------------------*/
struct Tux64FsFileLoadResult
tux64_fs_file_load(
   const char * path
);

/*----------------------------------------------------------------------------*/
/* Attempts to write a loaded file to the given path.                         */
/*----------------------------------------------------------------------------*/
struct Tux64FsResult
tux64_fs_file_save(
   const char * path,
   const struct Tux64FsLoadedFile * file
);

/*----------------------------------------------------------------------------*/
/* Frees the memory associated with a loaded file.                            */
/*----------------------------------------------------------------------------*/
void
tux64_fs_file_unload(
   struct Tux64FsLoadedFile * file
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_FS_ENABLE */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_FS_H */

