/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/pi.c - Implementations for Peripheral      */
/*    Interface (PI) helper functions.                                        */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/n64/pi.h"

Tux64PlatformMipsN64PiBusAddress
tux64_platform_mips_n64_pi_bus_address_dom1_free(
   Tux64UInt32 offset
) {
   return offset + TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_PI_BUS_DOMAIN_OFFSET_DOM1_FREE);
}

Tux64PlatformMipsN64PiBusAddress
tux64_platform_mips_n64_pi_bus_address_dom2_64dd_registers(
   Tux64UInt32 offset
) {
   return offset + TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_PI_BUS_DOMAIN_OFFSET_DOM2_64DD_REGISTERS);
}

Tux64PlatformMipsN64PiBusAddress
tux64_platform_mips_n64_pi_bus_address_dom1_64dd_rom(
   Tux64UInt32 offset
) {
   return offset + TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_PI_BUS_DOMAIN_OFFSET_DOM1_64DD_ROM);
}

Tux64PlatformMipsN64PiBusAddress
tux64_platform_mips_n64_pi_bus_address_dom2_sram(
   Tux64UInt32 offset
) {
   return offset + TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_PI_BUS_DOMAIN_OFFSET_DOM2_SRAM);
}

Tux64PlatformMipsN64PiBusAddress
tux64_platform_mips_n64_pi_bus_address_dom1_rom(
   Tux64UInt32 offset
) {
   return offset + TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_PI_BUS_DOMAIN_OFFSET_DOM1_ROM);
}

