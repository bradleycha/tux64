/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/memory-map.ld - CPU memory map linker      */
/*    script constants.                                                       */
/*----------------------------------------------------------------------------*/

/* MMU address segments which change how virtual addresses are mapped to */
/* physical addresses. */
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_USER_TLB
   = 0x00000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_CACHED
   = 0x80000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED
   = 0xa0000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_SUPERVISOR_TLB
   = 0xc0000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_KERNEL_TLB
   = 0xe0000000;

/* physical memory addresses which map directly to the console's address */
/* space. */
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RDRAM
   = 0x00000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RDRAM_REGISTERS
   = 0x03f00000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RDRAM_REGISTERS_BROADCAST
   = 0x03f80000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RSP_DMEM
   = 0x04000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RSP_IMEM
   = 0x04001000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RSP_REGISTERS
   = 0x04040000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RDP_REGISTERS
   = 0x04100000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_MI
   = 0x04300000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_VI
   = 0x04400000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_AI
   = 0x04500000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_PI
   = 0x04600000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RI
   = 0x04700000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_SI
   = 0x04800000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_64DD_REGISTERS
   = 0x05000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_64DD_IPL_ROM
   = 0x06000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_CARTRIDGE_RAM
   = 0x08000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_CARTRIDGE_ROM
   = 0x10000000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_PIF_ROM
   = 0x1fc00000;
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_PIF_RAM
   = 0x1fc007c0;

/* virtual addresses which can be used as an address from the CPU.  all of */
/* these will use the uncached direct segment, as user programs should never */
/* access these directly. RDRAM is the exception, with a cached segment */
/* available.  Only RDRAM supports cached access, so it's the only cached */
/* virtual address. */
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_CACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RDRAM
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_UNCACHED
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RDRAM
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_REGISTERS
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RDRAM_REGISTERS
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_REGISTERS_BROADCAST
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RDRAM_REGISTERS_BROADCAST
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RSP_DMEM
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_IMEM
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RSP_IMEM
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_REGISTERS
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RSP_REGISTERS
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDP_REGISTERS
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RDP_REGISTERS
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_MI
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_VI
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_AI
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_AI
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PI
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_PI
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_RI
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SI
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_SI
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_64DD_REGISTERS
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_64DD_REGISTERS
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_64DD_IPL_ROM
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_64DD_IPL_ROM
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_CARTRIDGE_RAM
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_CARTRIDGE_RAM
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_CARTRIDGE_ROM
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_CARTRIDGE_ROM
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PIF_ROM
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_PIF_ROM
   );
TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PIF_RAM
   = (
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SEGMENT_DIRECT_UNCACHED +
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PHYSICAL_PIF_RAM
   );

/* memory layout definitions for the above */
MEMORY
{
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED
      (rwx)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED,
      LENGTH = 0x03f00000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_UNCACHED
      (rwx)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_UNCACHED,
      LENGTH = 0x03f00000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_REGISTERS
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_REGISTERS,
      LENGTH = 0x00080000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_REGISTERS_BROADCAST
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_REGISTERS_BROADCAST,
      LENGTH = 0x00080000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_DMEM
      (rwx)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM,
      LENGTH = 0x00001000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_IMEM
      (rwx)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_IMEM,
      LENGTH = 0x00001000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_REGISTERS
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_REGISTERS,
      LENGTH = 0x00080000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDP_REGISTERS
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDP_REGISTERS,
      LENGTH = 0x00100000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_MI
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI,
      LENGTH = 0x00100000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_VI
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI,
      LENGTH = 0x00100000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_AI
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_AI,
      LENGTH = 0x00100000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_PI
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PI,
      LENGTH = 0x00100000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RI
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI,
      LENGTH = 0x00100000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_SI
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SI,
      LENGTH = 0x00100000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_64DD_REGISTERS
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_64DD_REGISTERS,
      LENGTH = 0x01000000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_64DD_IPL_ROM
      (r)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_64DD_IPL_ROM,
      LENGTH = 0x02000000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_CARTRIDGE_RAM
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_CARTRIDGE_RAM,
      LENGTH = 0x09000000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_CARTRIDGE_ROM
      (r)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_CARTRIDGE_ROM,
      LENGTH = 0x0fc00000
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_PIF_ROM
      (r)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PIF_ROM,
      LENGTH = 0x000007c0
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_PIF_RAM
      (rw)
      : ORIGIN = TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PIF_RAM,
      LENGTH = 0x00000040
}

