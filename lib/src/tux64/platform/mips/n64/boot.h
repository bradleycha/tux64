/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/boot.h - Common structs and types used by  */
/*    the bootloader.                                                         */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_BOOT_H
#define _TUX64_PLATFORM_MIPS_N64_BOOT_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/* the number of bytes per word. we choose this so all data will be 2-byte */
/* aligned, which is the smallest required alignment to work with PI DMA. */
#define TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD\
   2u

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
#define TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_MEMORY_DISPLAY\
   (1u << 1u)

struct Tux64PlatformMipsN64BootHeaderFileStage1 {
   Tux64UInt32 checksum;
   Tux64UInt32 length_words;
   Tux64UInt32 memory_words;
};

struct Tux64PlatformMipsN64BootHeaderFile {
   Tux64UInt32 checksum;
   Tux64UInt32 length_words;
};

struct Tux64PlatformMipsN64BootHeaderFileBootloader {
   struct Tux64PlatformMipsN64BootHeaderFileStage1 stage1;
   struct Tux64PlatformMipsN64BootHeaderFile stage2;
};

struct Tux64PlatformMipsN64BootHeaderFiles {
   struct Tux64PlatformMipsN64BootHeaderFileBootloader bootloader;
   struct Tux64PlatformMipsN64BootHeaderFile kernel;
   struct Tux64PlatformMipsN64BootHeaderFile initramfs;
   struct Tux64PlatformMipsN64BootHeaderFile command_line;
};

struct Tux64PlatformMipsN64BootHeaderData {
   /* bitflags used by various stages of the boot process */
   Tux64UInt32 flags;

   /* files used during the boot process */
   struct Tux64PlatformMipsN64BootHeaderFiles files;
};

/* the boot header stored after IPL3/stage-0 in ROM. */
struct Tux64PlatformMipsN64BootHeader {
   /* 4-byte magic value, used to quickly verify the boot header is present */
   Tux64UInt8 magic [TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_MAGIC_BYTES];

   /* checksum of the 'data' field */
   Tux64UInt32 checksum;

   /* boot header data which is protected by the checksum */
   struct Tux64PlatformMipsN64BootHeaderData data; 
};

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_BOOT_H */

