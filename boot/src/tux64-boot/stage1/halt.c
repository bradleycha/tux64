/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/halt.c - Implementation for halting the CPU.    */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/halt.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/vr4300/cop0.h>
#include <tux64/bitwise.h>

void
tux64_boot_stage1_halt(void) {
   /* this is basically the same code to halt the CPU in stage0.s, but now in */
   /* a *high level language*! */
   volatile Tux64UInt32 * halt_sysad;
   Tux64UInt32 status;

   /* bogus read to uncached memory to make sure SysAD isn't active */
   halt_sysad = (volatile Tux64UInt32 *)TUX64_LITERAL_UINTPTR(
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_UNCACHED
   );
   (void)(*halt_sysad);

   /* set the RIP bit in the co-processor 0 status register */
   status = tux64_platform_mips_vr4300_cop0_register_read_status();
   status = tux64_bitwise_flags_set_uint32(
      status,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_VR4300_COP0_STATUS_BIT_RP)
   );
   tux64_platform_mips_vr4300_cop0_register_write_status(status);

   /* our program now heads out to lie with the fishes. */
   TUX64_HALT;
   TUX64_UNREACHABLE;
}

