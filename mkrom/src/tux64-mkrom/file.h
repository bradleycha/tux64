/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/file.h - Header for file helpers.                    */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_MKROM_FILE_H
#define _TUX64_MKROM_FILE_H
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include <tux64/fs.h>

/*----------------------------------------------------------------------------*/
/* Loads a file from the given path.                                          */
/*----------------------------------------------------------------------------*/
struct Tux64FsFileLoadResult
tux64_mkrom_file_load(
   const struct Tux64String * path
);

/*----------------------------------------------------------------------------*/
/* Loads a file from the given path, prefixed by 'prefix'.                    */
/*----------------------------------------------------------------------------*/
struct Tux64FsFileLoadResult
tux64_mkrom_file_load_from_prefix(
   const struct Tux64String * path,
   const struct Tux64String * prefix
);

/*----------------------------------------------------------------------------*/
/* Writes a chunk of binary data to the given path.                           */
/*----------------------------------------------------------------------------*/
struct Tux64FsResult
tux64_mkrom_file_save(
   const struct Tux64String * path,
   const void * data,
   Tux64UInt32 bytes
);

struct Tux64FsResult
tux64_mkrom_file_load_result_to_fs_result(
   struct Tux64FsFileLoadResult file_load_result
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MKROM_FILE_H */

