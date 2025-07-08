/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.c - Stage-1 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

void
tux64_boot_stage1_start(
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_available
)
__attribute__((noreturn, section(".start"), externally_visible));

void
tux64_boot_stage1_start(
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_available
) {
   /* TODO: implement */
   (void)memory_total;
   (void)memory_available;
   TUX64_HALT;
}

