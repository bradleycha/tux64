/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/idle.c - Implementation for CPU idling and      */
/*    power management.                                                       */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/idle.h"

#include <tux64/platform/mips/vr4300/cop0.h>
#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/bitwise.h>

static void
tux64_boot_stage1_idle_rp_bit_flip(void) {
   volatile Tux64UInt32 * halt_sysad;
   Tux64UInt32 status;

   /* bogus read to uncached memory to make sure SysAD isn't active */
   halt_sysad = (volatile Tux64UInt32 *)TUX64_LITERAL_UINTPTR(
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_UNCACHED
   );
   (void)(*halt_sysad);

   /* flip the RIP bit in the co-processor 0 status register */
   status = tux64_platform_mips_vr4300_cop0_register_read_status();
   status = tux64_bitwise_flags_flip_uint32(
      status,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_VR4300_COP0_STATUS_BIT_RP)
   );
   tux64_platform_mips_vr4300_cop0_register_write_status(status);
}

void
tux64_boot_stage1_idle_enter(void) {
   /* normally we would have to configure the memory refresh rate if it were */
   /* based on pclock cycles, but RDRAM refresh, when set to automatic */
   /* refresh, refreshes based on the VI timings, *not* the CPU, so all we */
   /* have to do is set/unset the RP bit in COP0. */
   /* also, the CPU initially starts with RP=0, so we don't need to */
   /* initialize RP manually. */
   tux64_boot_stage1_idle_rp_bit_flip();
   return;
}

void
tux64_boot_stage1_idle_exit(void) {
   /* see above documentation about memory timings. */
   tux64_boot_stage1_idle_rp_bit_flip();
   return;
}

