/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/image.h - Header for ROM image creation.             */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_MKROM_IMAGE_H
#define _TUX64_MKROM_IMAGE_H
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/arguments.h"
#include "tux64-mkrom/load.h"

struct Tux64MkromImage {
   Tux64UInt8 * data;
   Tux64UInt32 bytes;
};

enum Tux64MkromImageBuildStatus {
   TUX64_MKROM_IMAGE_BUILD_STATUS_OK,
   TUX64_MKROM_IMAGE_BUILD_STATUS_OUT_OF_MEMORY,
   TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_BOOTLOADER_STAGE1,
   TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_BOOTLOADER_STAGE2,
   TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_KERNEL,
   TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_INITRAMFS,
   TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_ROOTFS,
   TUX64_MKROM_IMAGE_BUILD_STATUS_OVERFLOW_COMMAND_LINE
};

union Tux64MkromImageBuildPayload {
   struct Tux64MkromImage ok;
};

struct Tux64MkromImageBuildResult {
   enum Tux64MkromImageBuildStatus status;
   union Tux64MkromImageBuildPayload payload;
};

/*----------------------------------------------------------------------------*/
/* Builds a ROM image from the configuration and loaded data.  Upon success,  */
/* the ROM image must be freed with tux64_mkrom_image_free().                 */
/*----------------------------------------------------------------------------*/
struct Tux64MkromImageBuildResult
tux64_mkrom_image_build(
   const struct Tux64MkromArgumentsConfigFile * config,
   const struct Tux64MkromLoadData * data
);

/*----------------------------------------------------------------------------*/
/* Frees allocated memory for the given ROM image.                            */
/*----------------------------------------------------------------------------*/
void
tux64_mkrom_image_free(
   struct Tux64MkromImage * image
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MKROM_IMAGE_H */

