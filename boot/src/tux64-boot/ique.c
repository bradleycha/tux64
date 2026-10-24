/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/ique.c - Implementations for iQue platform code.       */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/ique.h"

#include <tux64/platform/mips/n64/mmio.h>

Tux64UInt32
tux64_boot_ique_random_bit(void) {
   Tux64UInt32 bit;

   bit = tux64_platform_mips_n64_mmio_registers_mi.ique_random;
   return bit;
}

