/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/kernel.h - Header for kernel ELF parser.             */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_MKROM_KERNEL_H
#define _TUX64_MKROM_KERNEL_H
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"

struct Tux64MkromKernelImage {
   const Tux64UInt8 * data;
   Tux64UInt32 bytes;
   Tux64UInt32 memory;
};

struct Tux64MkromKernel {
   struct Tux64MkromKernelImage image;
   Tux64UInt32 addr_load;
   Tux64UInt32 addr_entry;
   Tux64UInt32 alignment;
};

#define TUX64_MKROM_KERNEL_PARSE_STATUS_FIELD_COUNT 8u
enum Tux64MkromKernelParseStatus {
   TUX64_MKROM_KERNEL_PARSE_STATUS_OK,   
   TUX64_MKROM_KERNEL_PARSE_STATUS_CORRUPT_IMAGE,
   TUX64_MKROM_KERNEL_PARSE_STATUS_BAD_VERSION,
   TUX64_MKROM_KERNEL_PARSE_STATUS_INVALID_ENDIANESS,
   TUX64_MKROM_KERNEL_PARSE_STATUS_INVALID_TYPE,
   TUX64_MKROM_KERNEL_PARSE_STATUS_INVALID_MACHINE,
   TUX64_MKROM_KERNEL_PARSE_STATUS_MAIN_SEGMENT_MISSING,
   TUX64_MKROM_KERNEL_PARSE_STATUS_MAIN_SEGMENT_DUPLICATE
};

struct Tux64MkromKernelParsePayloadBadVersion {
   Tux64UInt32 version;
};

struct Tux64MkromKernelParsePayloadInvalidType {
   Tux64UInt32 type;
};

struct Tux64MkromKernelParsePayloadInvalidMachine {
   Tux64UInt32 machine;
};

union Tux64MkromKernelParsePayload {
   struct Tux64MkromKernel ok;
   struct Tux64MkromKernelParsePayloadBadVersion bad_version;
   struct Tux64MkromKernelParsePayloadInvalidType invalid_type;
   struct Tux64MkromKernelParsePayloadInvalidMachine invalid_machine;
};

struct Tux64MkromKernelParseResult {
   enum Tux64MkromKernelParseStatus status;
   union Tux64MkromKernelParsePayload payload;
};

/*----------------------------------------------------------------------------*/
/* Parses a kernel ELF image into its singular PT_LOAD segment and relevant   */
/* metadata.  Returned pointers have the same lifetime as the input pointers. */
/*----------------------------------------------------------------------------*/
struct Tux64MkromKernelParseResult
tux64_mkrom_kernel_parse(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MKROM_KERNEL_H */

