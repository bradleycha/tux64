/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/builder.h - Header for ROM builder.                  */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_MKROM_BUILDER_H
#define _TUX64_MKROM_BUILDER_H
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include <tux64/platform/mips/n64/boot.h>
#include <tux64/platform/mips/n64/rom.h>

#define TUX64_MKROM_BUILDER_MEASURE_STATUS_FIELD_COUNT 10u
enum Tux64MkromBuilderMeasureStatus {
   TUX64_MKROM_BUILDER_MEASURE_STATUS_OK,
   TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE0,
   TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE0_CIC,
   TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE1,
   TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE2,
   TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_KERNEL,
   TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_INITRAMFS,
   TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_COMMAND_LINE
};

struct Tux64MkromBuilderMeasurePayloadOk {
   struct Tux64PlatformMipsN64BootHeader boot_header;
   Tux64UInt32 rom_bytes;
};

union Tux64MkromBuilderMeasurePayload {
   struct Tux64MkromBuilderMeasurePayloadOk ok;
};

struct Tux64MkromBuilderMeasureResult {
   enum Tux64MkromBuilderMeasureStatus status;
   union Tux64MkromBuilderMeasurePayload payload;
};

struct Tux64MkromBuilderInputFile {
   const Tux64UInt8 * data;
   Tux64UInt32 bytes;
};

struct Tux64MkromBuilderInputFilesBootloader {
   struct Tux64MkromBuilderInputFile stage0;
   struct Tux64MkromBuilderInputFile stage0_cic;
   struct Tux64MkromBuilderInputFile stage1;
   struct Tux64MkromBuilderInputFile stage2;
};

struct Tux64MkromBuilderInputFiles {
   struct Tux64MkromBuilderInputFilesBootloader bootloader;
   struct Tux64MkromBuilderInputFile kernel;
   struct Tux64MkromBuilderInputFile initramfs;
};

struct Tux64MkromBuilderInput {
   struct Tux64MkromBuilderInputFiles files;
   struct Tux64PlatformMipsN64RomHeader rom_header;
   struct Tux64String kernel_command_line;
};

/*----------------------------------------------------------------------------*/
/* Measures how many bytes are required to create a ROM image.  This also     */
/* verifies all components can fit into memory and performs other checks.     */
/*----------------------------------------------------------------------------*/
struct Tux64MkromBuilderMeasureResult
tux64_mkrom_builder_measure_and_verify(
   const struct Tux64MkromBuilderInput * input
);

/*----------------------------------------------------------------------------*/
/* Constructs a ROM image after the input has been verified with              */
/* tux64_mkrom_builder_measure_and_verify().  'output' should be the number   */
/* of bytes return from the 'ok' variant of the previously mentioned          */
/* function.                                                                  */
/*----------------------------------------------------------------------------*/
void
tux64_mkrom_builder_construct(
   const struct Tux64MkromBuilderInput * input,
   const struct Tux64MkromBuilderMeasurePayloadOk * measure_info,
   Tux64UInt8 * output
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MKROM_BUILDER_H */

