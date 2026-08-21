/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/file.h - Header for file loading.               */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_FILE_H
#define _TUX64_BOOT_STAGE1_FILE_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/checksum.h>
#include <tux64/platform/mips/n64/pi.h>
#include <tux64/platform/mips/n64/boot.h>

enum Tux64BootStage1FileLoadPollStatus {
   TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_BUSY,
   TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_COMPLETE,
   TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_IO_ERROR,
   TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_BAD_CHECKSUM
};

/*----------------------------------------------------------------------------*/
/* The result of polling a file load operation.                               */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1FileLoadPollResult {
   enum Tux64BootStage1FileLoadPollStatus status;
   Tux64UInt32 bytes;
};

/*----------------------------------------------------------------------------*/
/* Stores information about an ongoing file transfer.                         */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1FileLoadContext {
   Tux64PlatformMipsN64PiBusAddress iter_addr_cart;
   Tux64UInt32 iter_addr_rdram;
   Tux64UInt32 bytes_remaining;
   union Tux64Int32 checksum_expected;
   struct Tux64ChecksumContext checksum_context;
};

/*----------------------------------------------------------------------------*/
/* Initializes a file transfer which can be polled with                       */
/* tux64_boot_stage1_file_load_poll(). 'load_address' must be aligned to an   */
/* 8 byte boundary.                                                           */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_file_load_initialize(
   struct Tux64BootStage1FileLoadContext * context,
   const struct Tux64PlatformMipsN64BootHeaderFile * file,
   Tux64UInt32 load_address
);

/*----------------------------------------------------------------------------*/
/* Attempts to poll a file transfer.  This should be called until 'status'    */
/* is not TUX64_BOOT_STAGE1_FILE_LOAD_POLL_STATUS_BUSY.  Once such a status   */
/* code is received, this function should no longer be called until a new     */
/* file transfer is started.                                                  */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1FileLoadPollResult
tux64_boot_stage1_file_load_poll(
   struct Tux64BootStage1FileLoadContext * context
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_FILE_H */

