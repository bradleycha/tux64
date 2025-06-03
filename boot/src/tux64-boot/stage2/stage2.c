/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.c - Stage-2 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

#include <tux64/platform-n64/boot.h>
#include "tux64-boot/stage3/payload.h"

void
tux64_boot_stage2_start(
   const struct Tux64PlatformN64BootHeader *,
   Tux64UInt32
)
__attribute__((noreturn, section(".start")));

void
tux64_boot_stage2_start(
   const struct Tux64PlatformN64BootHeader * boot_header,
   Tux64UInt32 total_memory
) {
   /* TODO: implement */
   (void)boot_header;
   (void)total_memory;
   for (;;) {}
}

