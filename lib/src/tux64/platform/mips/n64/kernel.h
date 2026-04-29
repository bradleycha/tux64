/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64-lib/platform/mips/n64/kernel.h - Header for N64 kernel image */
/*    functions.                                                              */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_KERNEL_H
#define _TUX64_PLATFORM_MIPS_N64_KERNEL_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/*----------------------------------------------------------------------------*/
/* Describes the location of the kernel image data which needs to be loaded   */
/* into memory.                                                               */
/*----------------------------------------------------------------------------*/
/* offset - The location within the file which contains the start of the      */
/*    kernel image data.                                                      */
/*                                                                            */
/* bytes - The length of the kernel image data, in bytes.                     */
/*----------------------------------------------------------------------------*/
struct Tux64PlatformMipsN64KernelImage {
   Tux64UInt32 offset;
   Tux64UInt32 bytes;
};

/*----------------------------------------------------------------------------*/
/* Contains the data necessary to load and execute a kernel.                  */
/*----------------------------------------------------------------------------*/
/* image - The raw data which needs to be loaded into memory.                 */
/*                                                                            */
/* memory - The amount of memory required by the kernel to execute.  This     */
/*    will always be at least as long as the kernel image.                    */
/*                                                                            */
/* addr_load - Where in memory the kernel image must be loaded.               */
/*                                                                            */
/* addr_entry - The address which contains the start of executable code.      */
/*----------------------------------------------------------------------------*/
struct Tux64PlatformMipsN64Kernel {
   struct Tux64PlatformMipsN64KernelImage image;
   Tux64UInt32 memory;
   Tux64UInt32 addr_load;
   Tux64UInt32 addr_entry;
};

#define TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_FIELD_COUNT 8u
enum Tux64PlatformMipsN64KernelParseStatus {
   TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_OK,   
   TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_CORRUPT_IMAGE,
   TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_BAD_VERSION,
   TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_INVALID_ENDIANESS,
   TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_INVALID_TYPE,
   TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_INVALID_MACHINE,
   TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_MISSING,
   TUX64_PLATFORM_MIPS_N64_KERNEL_PARSE_STATUS_MAIN_SEGMENT_DUPLICATE
};

struct Tux64PlatformMipsN64KernelParsePayloadBadVersion {
   Tux64UInt32 version;
};

struct Tux64PlatformMipsN64KernelParsePayloadInvalidType {
   Tux64UInt32 type;
};

struct Tux64PlatformMipsN64KernelParsePayloadInvalidMachine {
   Tux64UInt32 machine;
};

union Tux64PlatformMipsN64KernelParsePayload {
   struct Tux64PlatformMipsN64Kernel ok;
   struct Tux64PlatformMipsN64KernelParsePayloadBadVersion bad_version;
   struct Tux64PlatformMipsN64KernelParsePayloadInvalidType invalid_type;
   struct Tux64PlatformMipsN64KernelParsePayloadInvalidMachine invalid_machine;
};

struct Tux64PlatformMipsN64KernelParseResult {
   enum Tux64PlatformMipsN64KernelParseStatus status;
   union Tux64PlatformMipsN64KernelParsePayload payload;
};

/*----------------------------------------------------------------------------*/
/* Parses a kernel ELF image into its singular PT_LOAD segment and relevant   */
/* metadata.  Returned pointers have the same lifetime as the input pointers. */
/*----------------------------------------------------------------------------*/
struct Tux64PlatformMipsN64KernelParseResult
tux64_platform_mips_n64_kernel_parse(
   const Tux64UInt8 * elf_data,
   Tux64UInt32 elf_bytes
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_KERNEL_H */

