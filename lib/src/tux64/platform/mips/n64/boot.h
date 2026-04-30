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
/* in addition, being 4-byte aligned means we don't have to deal with */
/* unaligned or residual data when checksumming on the console, which */
/* massively simplifies the code there. */
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
#define TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_MEMORY_DISPLAY\
   (1u << 1u)
#define TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_NO_DELAY\
   (1u << 2u)

struct Tux64PlatformMipsN64BootHeaderFile {
   Tux64UInt32 checksum;
   Tux64UInt32 addr_cart;
   Tux64UInt32 length;
};

struct Tux64PlatformMipsN64BootHeaderExecutable {
   struct Tux64PlatformMipsN64BootHeaderFile file;
   Tux64UInt32 memory;
};

/* we need an exception here because we get 4 bytes of bloat from addr_cart,  */
/* which is always cart_addr+0x1000+sizeof(boot header). */
struct Tux64PlatformMipsN64BootHeaderFileStage1 {
   Tux64UInt32 checksum;
   Tux64UInt32 length;
   Tux64UInt32 memory;
};

struct Tux64PlatformMipsN64BootHeaderFileBootloader {
   struct Tux64PlatformMipsN64BootHeaderFileStage1 stage1;
   struct Tux64PlatformMipsN64BootHeaderFile stage2;
};

struct Tux64PlatformMipsN64BootHeaderFileKernel {
   struct Tux64PlatformMipsN64BootHeaderExecutable image;
   Tux64UInt32 addr_load;
   Tux64UInt32 addr_entry;
};

struct Tux64PlatformMipsN64BootHeaderFiles {
   struct Tux64PlatformMipsN64BootHeaderFileBootloader bootloader;
   struct Tux64PlatformMipsN64BootHeaderFileKernel kernel;
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

