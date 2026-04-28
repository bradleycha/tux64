/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/boot-header.c - Implementations for globally    */
/*    loaded boot header functions.                                           */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/boot-header.h"

#include <tux64/platform/mips/n64/boot.h>
#include <tux64/platform/mips/n64/pi.h>
#include <tux64/platform/mips/n64/rom.h>
#include <tux64/bitwise.h>
#include <tux64/math.h>

static Tux64Boolean
tux64_boot_stage1_boot_header_flag(
   Tux64UInt32 bit
) {
   return tux64_bitwise_flags_check_one_uint32(
      tux64_boot_stage1_boot_header.data.flags,
      bit
   );
}

Tux64Boolean
tux64_boot_stage1_boot_header_flag_no_checksum(void) {
   return tux64_boot_stage1_boot_header_flag(TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_NO_CHECKSUM);
}

Tux64Boolean
tux64_boot_stage1_boot_header_flag_memory_display(void) {
   return tux64_boot_stage1_boot_header_flag(TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_MEMORY_DISPLAY);
}

static Tux64PlatformMipsN64PiBusAddress
tux64_boot_stage1_boot_header_align_pi_address(
   Tux64PlatformMipsN64PiBusAddress address
) {
   return tux64_math_align_forward_uint32(
      address,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD)
   );
}

struct Tux64BootStage1BootHeaderFiles
tux64_boot_stage1_boot_header_files(void) {
   const struct Tux64PlatformMipsN64BootHeaderFiles * header;
   Tux64UInt32 bytes_bootloader_stage1;
   Tux64UInt32 bytes_bootloader_stage2;
   Tux64UInt32 bytes_kernel_image;
   Tux64UInt32 bytes_kernel_memory;
   Tux64UInt32 bytes_initramfs;
   Tux64UInt32 bytes_command_line;
   struct Tux64BootStage1BootHeaderFiles files;
   Tux64PlatformMipsN64PiBusAddress address;

   header = &tux64_boot_stage1_boot_header.data.files;

   bytes_bootloader_stage1 = (header->bootloader.stage1.length_words * TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD));
   bytes_bootloader_stage2 = (header->bootloader.stage2.length_words * TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD));
   bytes_kernel_image      = (header->kernel.image.length_words      * TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD));
   bytes_kernel_memory     = (header->kernel.image.memory_words      * TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD));
   bytes_initramfs         = (header->initramfs.length_words         * TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD));
   bytes_command_line      = (header->command_line.length_words      * TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_BOOT_BYTES_PER_WORD));

   address = tux64_platform_mips_n64_pi_bus_address_dom1_rom(TUX64_LITERAL_UINT32(0x00001000u));
   address = tux64_boot_stage1_boot_header_align_pi_address(address + TUX64_LITERAL_UINT32(sizeof(struct Tux64PlatformMipsN64BootHeader)));
   address = tux64_boot_stage1_boot_header_align_pi_address(address + bytes_bootloader_stage1);

   files.bootloader_stage2.address  = address;
   files.bootloader_stage2.bytes    = bytes_bootloader_stage2;
   files.bootloader_stage2.checksum = header->bootloader.stage2.checksum;
   address = tux64_boot_stage1_boot_header_align_pi_address(address + bytes_bootloader_stage2);

   files.kernel.file.address  = address;
   files.kernel.file.bytes    = bytes_kernel_image;
   files.kernel.file.checksum = header->kernel.image.checksum;
   files.kernel.memory        = bytes_kernel_memory;
   address = tux64_boot_stage1_boot_header_align_pi_address(address + bytes_kernel_image);

   files.initramfs.address    = address;
   files.initramfs.bytes      = bytes_initramfs;
   files.initramfs.checksum   = header->initramfs.checksum;
   address = tux64_boot_stage1_boot_header_align_pi_address(address + bytes_initramfs);

   files.command_line.address    = address;
   files.command_line.bytes      = bytes_command_line;
   files.command_line.checksum   = header->command_line.checksum;
   address = tux64_boot_stage1_boot_header_align_pi_address(address + bytes_command_line);

   return files;
}

