/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/load.h - Header for boot file loading and parsing.   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_MKROM_LOAD_H
#define _TUX64_MKROM_LOAD_H
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/arguments.h"
#include <tux64/fs.h>
#include <tux64/parse/string-integer.h>
#include <tux64/platform/mips/n64/kernel.h>

#define TUX64_MKROM_LOAD_DATA_BOOTLOADER_STAGE0_BYTES\
   4032u /* 1KiB - sizeof(rom header) */
#define TUX64_MKROM_LOAD_DATA_BOOTLOADER_STAGE0_CIC_BYTES\
   8u

struct Tux64MkromLoadDataBootloader {
   Tux64UInt8 stage0 [TUX64_MKROM_LOAD_DATA_BOOTLOADER_STAGE0_BYTES];
   struct Tux64FsLoadedFile stage1;
   Tux64UInt32 stage1_bss;
   struct Tux64FsLoadedFile stage2;
   Tux64UInt32 flags;
};

struct Tux64MkromLoadData {
   struct Tux64MkromLoadDataBootloader bootloader;
   struct Tux64FsLoadedFile kernel_file; /* needed to preserve lifetime with parsed kernel below. */
   struct Tux64PlatformMipsN64Kernel kernel;
   struct Tux64FsLoadedFile initramfs;
   struct Tux64FsLoadedFile rootfs;
};

enum Tux64MkromLoadStatus {
   TUX64_MKROM_LOAD_STATUS_OK,
   TUX64_MKROM_LOAD_STATUS_OUT_OF_MEMORY,
   TUX64_MKROM_LOAD_STATUS_FS_ERROR,
   TUX64_MKROM_LOAD_STATUS_INVALID_STAGE0_LENGTH,
   TUX64_MKROM_LOAD_STATUS_INVALID_STAGE0_CIC_LENGTH,
   TUX64_MKROM_LOAD_STATUS_INVALID_STAGE1_ALIGNMENT,
   TUX64_MKROM_LOAD_STATUS_INVALID_STAGE1_BSS,
   TUX64_MKROM_LOAD_STATUS_KERNEL_PARSE_ERROR
};

struct Tux64MkromLoadPayloadFsError {
   struct Tux64FsResult reason;
};

struct Tux64MkromLoadPayloadInvalidStage0Length {
   Tux64UInt32 length_provided;
   Tux64UInt32 length_maximum;
};

struct Tux64MkromLoadPayloadInvalidStage0CicLength {
   Tux64UInt32 length_provided;
   Tux64UInt32 length_expected;
};

struct Tux64MkromLoadPayloadInvalidStage1Alignment {
   Tux64UInt32 length_provided;
   Tux64UInt32 alignment;
   Tux64UInt32 residual;
};

struct Tux64MkromLoadPayloadInvalidStage1Bss {
   struct Tux64ParseStringIntegerResult reason;
};

struct Tux64MkromLoadPayloadKernelParseError {
   struct Tux64PlatformMipsN64KernelParseResult reason;
};

union Tux64MkromLoadPayload {
   struct Tux64MkromLoadData ok;
   struct Tux64MkromLoadPayloadFsError fs_error;
   struct Tux64MkromLoadPayloadInvalidStage0Length invalid_stage0_length;
   struct Tux64MkromLoadPayloadInvalidStage0CicLength invalid_stage0_cic_length;
   struct Tux64MkromLoadPayloadInvalidStage1Alignment invalid_stage1_alignment;
   struct Tux64MkromLoadPayloadInvalidStage1Bss invalid_stage1_bss;
   struct Tux64MkromLoadPayloadKernelParseError kernel_parse_error;
};

struct Tux64MkromLoadResult {
   enum Tux64MkromLoadStatus status;
   union Tux64MkromLoadPayload payload;
};

/*----------------------------------------------------------------------------*/
/* Attempts to load and parse all boot files.  Upon success, memory will be   */
/* allocated, which must be freed by calling tux64_mkrom_load_free_data().    */
/*----------------------------------------------------------------------------*/
struct Tux64MkromLoadResult
tux64_mkrom_load_data(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64String * file_path_prefix
);

/*----------------------------------------------------------------------------*/
/* Frees all memory allocated in the load data struct.                        */
/*----------------------------------------------------------------------------*/
void
tux64_mkrom_load_free_data(
   struct Tux64MkromLoadData * data
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MKROM_LOAD_H */

