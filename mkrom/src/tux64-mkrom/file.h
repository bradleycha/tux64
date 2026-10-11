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
/* Loads a file from the given path, prefixed by 'prefix'.                    */
/*----------------------------------------------------------------------------*/
struct Tux64FsFileLoadResult
tux64_mkrom_file_load_from_prefix(
   const struct Tux64String * path,
   const struct Tux64String * prefix
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MKROM_FILE_H */

