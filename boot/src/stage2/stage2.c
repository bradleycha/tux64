/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage2/stage2.c - Stage-2 bootloader main entrypoint.             */
/*----------------------------------------------------------------------------*/

#include "tux64-boot.h"

#include "stage2/payload.h"

void
tux64_boot_stage2_start(void)
__attribute__((noreturn, section(".start")));

void
tux64_boot_stage2_start(void) {
   /* TODO: implement */
   for (;;) {}
}

