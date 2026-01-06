/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/strings.h - Header for fbcon text strings.      */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_STRINGS_H
#define _TUX64_BOOT_STAGE1_STRINGS_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/fbcon.h"
#include "tux64-boot/stage1/format.h"

extern const struct Tux64BootStage1FbconText
tux64_boot_stage1_strings_splash;
extern const struct Tux64BootStage1FbconText
tux64_boot_stage1_strings_memory_total;
extern const struct Tux64BootStage1FbconText
tux64_boot_stage1_strings_memory_free;
extern const struct Tux64BootStage1FbconText
tux64_boot_stage1_strings_kernel_image;
extern const struct Tux64BootStage1FbconText
tux64_boot_stage1_strings_initramfs_image;
extern const struct Tux64BootStage1FbconText
tux64_boot_stage1_strings_no_checksum;
extern const struct Tux64BootStage1FbconText
tux64_boot_stage1_strings_hello_world;

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_STRINGS_H */

