/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/start.c - Stage-2 bootloader startup code.      */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage2/stage2.h"

#include "tux64-boot/load.h"

void
tux64_boot_stage2_start(void)
__attribute__((noreturn, section(".start"), externally_visible));

void
tux64_boot_stage2_start(void) {
   /* same idea as stage-1, which ensures a stable ABI across boot stages. */
   register Tux64UInt32          memory_total   __asm__("$s0");
   register Tux64BootLoadStatus  load_status    __asm__("$s1");

   tux64_boot_stage2_main(
      memory_total,
      load_status
   );
   TUX64_UNREACHABLE;
}

