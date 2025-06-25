/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/boot.h - Common structs and types used by  */
/*    the bootloader.                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_BOOT_H
#define _TUX64_PLATFORM_MIPS_N64_BOOT_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/* the number of bytes per word */
#define TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD\
   4u

#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_MAGIC\
   "TBHM" /* Tux64 Boot Header Magic */

#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_MAGIC_BYTES\
   sizeof(Tux64UInt32)

struct Tux64PlatformMipsN64BootHeaderVersion {
   Tux64UInt8 major;
   Tux64UInt8 minor;
   Tux64UInt16 patch;
};

#define TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_NO_CHECKSUM\
   (1u << 0u)

struct Tux64PlatformMipsN64BootHeaderFileBootloaderStage1 {
   Tux64UInt32 checksum;
   Tux64UInt32 length_words_code_data;
};

struct Tux64PlatformMipsN64BootHeaderFileBootloaderStage2 {
   Tux64UInt32 checksum;
   Tux64UInt32 length_words_code_data;
   Tux64UInt32 length_words_bss;
};

struct Tux64PlatformMipsN64BootHeaderFileBootloaderStage3 {
   Tux64UInt32 checksum;
   Tux64UInt32 length_words_code_data;
};

struct Tux64PlatformMipsN64BootHeaderFileKernel {
   Tux64UInt32 checksum;
   Tux64UInt32 length_words_code_data;
   Tux64UInt32 length_words_bss;
};

struct Tux64PlatformMipsN64BootHeaderFileInitramfs {
   Tux64UInt32 checksum;
   Tux64UInt32 length_words;
};

struct Tux64PlatformMipsN64BootHeaderFileCommandLine {
   Tux64UInt32 checksum;
   Tux64UInt32 length_words;
};

struct Tux64PlatformMipsN64BootHeaderFileBootloader {
   struct Tux64PlatformMipsN64BootHeaderFileBootloaderStage1 stage1;
   struct Tux64PlatformMipsN64BootHeaderFileBootloaderStage2 stage2;
   struct Tux64PlatformMipsN64BootHeaderFileBootloaderStage3 stage3;
};

struct Tux64PlatformMipsN64BootHeaderFiles {
   struct Tux64PlatformMipsN64BootHeaderFileBootloader bootloader;
   struct Tux64PlatformMipsN64BootHeaderFileKernel kernel;
   struct Tux64PlatformMipsN64BootHeaderFileInitramfs initramfs;
   struct Tux64PlatformMipsN64BootHeaderFileCommandLine command_line;
};

/* the boot header stored after IPL3/stage-0 in ROM. */
struct Tux64PlatformMipsN64BootHeader {
   /* 4-byte magic value, used to quickly verify the boot header is present */
   Tux64UInt8 magic [TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_MAGIC_BYTES];

   /* checksum of the entire header, minus the magic value, used to verify */
   /* the header isn't corrupted */
   Tux64UInt32 checksum;

   /* the bootloader version associated with the header */
   struct Tux64PlatformMipsN64BootHeaderVersion version;

   /* the length of the header, in words */
   Tux64UInt32 length_words;

   /* bitflags used by various stages of the boot process */
   Tux64UInt32 flags;

   /* files used during the boot process */
   struct Tux64PlatformMipsN64BootHeaderFiles files;
};

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_BOOT_H */

