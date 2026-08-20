/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/file.h - File loading interface.                */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE2_FILE_H
#define _TUX64_BOOT_STAGE2_FILE_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/load.h"

/*----------------------------------------------------------------------------*/
/* Loads all boot files from cartridge into memory which were unable to be    */
/* loaded from stage1.  This sets status codes and verifies checksums, if     */
/* enabled.                                                                   */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage2_file_load_on_demand(
   Tux64BootLoadStatus load_status
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_FILE_H */

