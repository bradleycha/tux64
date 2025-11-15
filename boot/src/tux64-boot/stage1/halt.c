/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/halt.c - Implementation for halting the CPU.    */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/halt.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include "tux64-boot/stage1/idle.h"

void
tux64_boot_stage1_halt(void) {
   Tux64UIntPtr bogus;

   /* we idle the CPU because we want to ~efficiently~ crash! no performance */
   /* left on the table, even as our program takes its last breaths! */
   tux64_boot_stage1_idle_enter();

   /* we use the same trick from stage-0 where we deliberately make an        */
   /* invalid 64-bit read from RSP memory.  we use __asm__ to ensure we       */
   /* always use a 'ld' instruction instead of potentially using two 'lw'     */
   /* instructions, which could happen with -mabi=32 (why would you do that?) */
   bogus = TUX64_LITERAL_UINTPTR(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM);
   __asm__ volatile ("ld $zero,0(%0)" :: "r" (bogus));

   TUX64_UNREACHABLE;
}

