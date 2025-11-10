/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/halt.c - Implementation for halting the CPU.    */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/halt.h"

#include "tux64-boot/stage1/idle.h"

void
tux64_boot_stage1_halt(void) {
   /* we idle the CPU because we want to ~efficiently~ crash! no performance */
   /* left on the table, even as our program takes its last breaths! */
   tux64_boot_stage1_idle_enter();
   TUX64_HALT;
   TUX64_UNREACHABLE;
}

