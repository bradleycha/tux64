/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage3/stage3.c - Stage-3 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

#include "tux64-boot/stage3/payload.h"

void
tux64_boot_stage3_start(void)
__attribute__((noreturn, section(".start")));

void
tux64_boot_stage3_start(void) {
   /* TODO: implement */
   for (;;) {}
}

