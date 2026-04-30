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

extern const struct Tux64PlatformMipsN64BootHeader
tux64_boot_stage1_boot_header
__attribute__((section(".boot_header")));

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

Tux64Boolean
tux64_boot_stage1_boot_header_flag_no_delay(void) {
   return tux64_boot_stage1_boot_header_flag(TUX64_PLATFORM_MIPS_N64_BOOT_FLAG_NO_DELAY);
}

const struct Tux64PlatformMipsN64BootHeaderFile *
tux64_boot_stage1_boot_header_file_bootloader_stage2(void) {
   return &tux64_boot_stage1_boot_header.data.files.bootloader.stage2;
}

const struct Tux64PlatformMipsN64BootHeaderFileKernel *
tux64_boot_stage1_boot_header_file_kernel(void) {
   return &tux64_boot_stage1_boot_header.data.files.kernel;
}

const struct Tux64PlatformMipsN64BootHeaderFile *
tux64_boot_stage1_boot_header_file_initramfs(void) {
   return &tux64_boot_stage1_boot_header.data.files.initramfs;
}

const struct Tux64PlatformMipsN64BootHeaderFile *
tux64_boot_stage1_boot_header_file_command_line(void) {
   return &tux64_boot_stage1_boot_header.data.files.command_line;
}

