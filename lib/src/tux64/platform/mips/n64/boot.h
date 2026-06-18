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
#include "tux64/math.h"

/* the number of bytes per word. we choose this so all data will be 2-byte */
/* aligned, which is the smallest required alignment to work with PI DMA. */
/* in addition, being 4-byte aligned means we don't have to deal with */
/* unaligned or residual data when checksumming on the console, which */
/* massively simplifies the code there. */
#define TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD\
   4u

#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_MAGIC\
   (0x5442484d) /* TBHM (Tux64 Boot Header Magic) */

#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_MAGIC_BYTES\
   (4u)

#define TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_NO_CHECKSUM\
   (1u << 0u)
#define TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_MEMORY_DISPLAY\
   (1u << 1u)
#define TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_NO_DELAY\
   (1u << 2u)

/*----------------------------------------------------------------------------*/
/* The size of the boot header, in bytes.  Also provides a version aligned    */
/* forward to the nearest 16-byte boundary.                                   */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES\
   (0x54u)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES_ALIGN16\
   (TUX64_MATH_ALIGN_FORWARD(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES, 16))

/*----------------------------------------------------------------------------*/
/* The offset of each field in the boot header.                               */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_MAGIC\
   (0u)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_CHECKSUM\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_MAGIC)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_CHECKSUM)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FLAGS\
   (0u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_CHECKSUM\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FLAGS)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_LENGTH\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_CHECKSUM)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_MEMORY\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_LENGTH)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE2_CHECKSUM\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_MEMORY)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE2_ADDR_CART\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE2_CHECKSUM)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE2_LENGTH\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE2_ADDR_CART)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_CHECKSUM\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE2_LENGTH)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_ADDR_CART\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_CHECKSUM)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_LENGTH\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_ADDR_CART)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_MEMORY\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_LENGTH)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_ADDR_LOAD\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_MEMORY)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_ADDR_ENTRY\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_ADDR_LOAD)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_INITRAMFS_CHECKSUM\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_ADDR_ENTRY)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_INITRAMFS_ADDR_CART\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_INITRAMFS_CHECKSUM)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_INITRAMFS_LENGTH\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_INITRAMFS_ADDR_CART)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_COMMAND_LINE_CHECKSUM\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_INITRAMFS_LENGTH)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_COMMAND_LINE_ADDR_CART\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_COMMAND_LINE_CHECKSUM)
#define TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_COMMAND_LINE_LENGTH\
   (4u + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_COMMAND_LINE_ADDR_CART)

#if !TUX64_PREPROCESSOR_ONLY
/*----------------------------------------------------------------------------*/

struct Tux64PlatformMipsN64BootHeaderVersion {
   Tux64UInt8 major;
   Tux64UInt8 minor;
   Tux64UInt16 patch;
};

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

/* since we have to set stuff manually to work with assembler includes, it's */
/* a good idea to make sure we don't have any issues. */

TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES == sizeof(struct Tux64PlatformMipsN64BootHeader));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES_ALIGN16 % 16 == 0);
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES_ALIGN16 < 16 + TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES);

TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_MAGIC                               == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, magic));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_CHECKSUM                            == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, checksum));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA                                == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FLAGS                          == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.flags));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_CHECKSUM          == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.bootloader.stage1.checksum));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_LENGTH            == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.bootloader.stage1.length));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE1_MEMORY            == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.bootloader.stage1.memory));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE2_CHECKSUM          == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.bootloader.stage2.checksum));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE2_ADDR_CART         == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.bootloader.stage2.addr_cart));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_STAGE2_LENGTH            == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.bootloader.stage2.length));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_CHECKSUM    == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.kernel.image.file.checksum));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_ADDR_CART   == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.kernel.image.file.addr_cart));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_LENGTH      == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.kernel.image.file.length));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_IMAGE_MEMORY      == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.kernel.image.memory));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_ADDR_LOAD         == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.kernel.addr_load));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_KERNEL_ADDR_ENTRY        == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.kernel.addr_entry));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_INITRAMFS_CHECKSUM       == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.initramfs.checksum));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_INITRAMFS_ADDR_CART      == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.initramfs.addr_cart));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_INITRAMFS_LENGTH         == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.initramfs.length));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_COMMAND_LINE_CHECKSUM    == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.command_line.checksum));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_COMMAND_LINE_ADDR_CART   == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.command_line.addr_cart));
TUX64_ASSERT_STATIC(TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_OFFSET_DATA_FILES_COMMAND_LINE_LENGTH      == TUX64_OFFSET_OF(struct Tux64PlatformMipsN64BootHeader, data.files.command_line.length));

/*----------------------------------------------------------------------------*/
#endif /* !TUX64_PREPROCESSOR_ONLY */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_BOOT_H */

