/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/halt.c - Implementation for halting the CPU.           */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/halt.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include "tux64-boot/idle.h"

#define TUX64_BOOT_HALT_UNMAPPED_ADDRESS \
   (TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED + 0x09000000u)

void
tux64_boot_halt(void) {
   volatile const Tux64UInt8 * bogus;

   /* we idle the CPU because we want to ~efficiently~ crash! no performance */
   /* left on the table, even as our program takes its last breaths! */
   tux64_boot_idle_enter();

   /* we now use a similar but different trick from stage-0 to halt the CPU. */
   /* this works by attempting to read any amount of data from an unmapped */
   /* section of memory, which causes the CPU to hang indefinitely.  the */
   /* benefit to doing it here is it work with any ABI (with -mabi=32, a */
   /* 64-bit read would be split into two 32-bit reads) without needing */
   /* inline asm.  the reason we don't do this in stage-0 is because we can */
   /* make use of $gp, which is loaded with the address to RSP DMEM, to save */
   /* an instruction.  we don't have this here, so this doesn't work. */
   bogus = (volatile const Tux64UInt8 *)TUX64_LITERAL_UINTPTR(TUX64_BOOT_HALT_UNMAPPED_ADDRESS);
   (void)(*bogus);

   TUX64_UNREACHABLE;
}

