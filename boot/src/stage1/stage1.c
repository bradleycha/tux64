/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage1/stage1.c - Stage-1 bootloader main entrypoint.             */
/*----------------------------------------------------------------------------*/

#include "tux64_boot.h"

#include <tux64/platform_n64/boot.h>
#include "stage2/payload.h"

void
tux64_boot_stage1_start(
   const struct Tux64PlatformN64BootHeader *,
   Tux64UInt32
)
__attribute__((noreturn, section(".start")));

void
tux64_boot_stage1_start(
   const struct Tux64PlatformN64BootHeader * boot_header,
   Tux64UInt32 total_memory
) {
   /* TODO: implement */
   (void)boot_header;
   (void)total_memory;
   for (;;) {}
}

