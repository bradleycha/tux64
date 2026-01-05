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
#include <tux64/bitwise.h>

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

