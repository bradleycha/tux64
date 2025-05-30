/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage1/stage1.c - Stage-1 bootloader main entrypoint.             */
/*----------------------------------------------------------------------------*/

#include <tux64/tux64.h>
#include <tux64/platform_n64/mmio.h>
#include <tux64/platform_n64/boot.h>

void
tux64_boot_stage1_start(void)
__attribute__((noreturn, section(".start")));

void
tux64_boot_stage1_start(void) {
   /* TODO: implement */
   tux64_platform_n64_mmio_registers_mi.mode = sizeof(struct Tux64PlatformN64BootHeader);
   for (;;) {}
}

