/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* develop/headers/platform_n64/boot.h - Common structs and types used by the */
/*    bootloader.                                                             */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_N64_BOOT_H
#define _TUX64_PLATFORM_N64_BOOT_H
/*------------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/* stores the length of various boot items, used by all stages of the */
/* bootloader. */
struct Tux64PlatformN64BootItemLengths {
   Tux64UInt32 kernel;
   Tux64UInt32 initramfs;
   Tux64UInt16 command_line;
   Tux64UInt16 bootloader_stage2;
   Tux64UInt32 bootloader_stage1;
};

/* stores information about the current hardware configuration. this is */
/* created by the stage-0 bootloader and passed to the stage-1 bootloader. */
struct Tux64PlatformN64BootHardwareConfiguration {
   struct Tux64PlatformN64BootItemLengths item_lengths;

   /* the total amount of detected RDRAM memory, in bytes */
   Tux64UInt32 available_rdram;
};

struct Tux64PlatformN64BootVersion {
   Tux64UInt8 major;
   Tux64UInt8 minor;
   Tux64UInt16 revision;
};

#define TUX64_BOOT_HEADER_MAGIC_BYTES\
   (8u)
#define TUX64_BOOT_HEADER_MAGIC\
   "TUX64LDR"

/* the boot header stored in cartridge ROM directly after the stage-0 */
/* bootloader. note that byte offsets and lengths are postfixed with "div4". */
/* this means that the number stored in the bytes are a quarter of their */
/* actual value.  this is done because we align all boot items to a word */
/* boundary to both make it compatible with RSP DMA, as well as make memcpy */
/* more efficient.  this also makes it impossible to pass a misaligned offset */
/* to the bootloader. */
struct Tux64PlatformN64BootHeader {
   /* this should always be TUX64_BOOT_HEADER_MAGIC */
   Tux64UInt8 magic [TUX64_BOOT_HEADER_MAGIC_BYTES];

   struct Tux64PlatformN64BootVersion version;
   struct Tux64PlatformN64BootItemLengths item_lengths_d4;

   /* the offset from cartridge ROM + 0x1000 where to find all the boot items */
   /* located in cartridge ROM. */
   Tux64UInt16 offset_d4;

   /* miscellaneous bitflags used to control the boot process */
   Tux64UInt16 flags;
};

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_H */

