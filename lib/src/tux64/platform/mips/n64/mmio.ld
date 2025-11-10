/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/mmio.ld - Memory-mapped I/O (MMIO) linker  */
/*    script to place MMIO structs in the correct location.                   */
/*----------------------------------------------------------------------------*/

SECTIONS
{
   /* RSP Registers */
   .mmio.sp TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_REGISTERS : {
      tux64_platform_mips_n64_mmio_registers_sp = .;
      *(.tux64.platform.mips.n64.mmio.sp);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RSP_REGISTERS

   /* MIPS Interface (MI) */
   .mmio.mi TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_MI : {
      tux64_platform_mips_n64_mmio_registers_mi = .;
      *(.tux64.platform.mips.n64.mmio.mi);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_MI

   /* Video Interface (VI) */
   .mmio.vi TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_VI : {
      tux64_platform_mips_n64_mmio_registers_vi = .;
      *(.tux64.platform.mips.n64.mmio.vi);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_VI

   /* Audio Interface (AI) */
   .mmio.ai TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_AI : {
      tux64_platform_mips_n64_mmio_registers_ai = .;
      *(.tux64.platform.mips.n64.mmio.ai);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_AI

   /* Peripheral Interface (PI) */
   .mmio.pi TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PI : {
      tux64_platform_mips_n64_mmio_registers_pi = .;
      *(.tux64.platform.mips.n64.mmio.pi);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_PI

   /* RDRAM Interface (RI) */
   .mmio.ri TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RI : {
      tux64_platform_mips_n64_mmio_registers_ri = .;
      *(.tux64.platform.mips.n64.mmio.ri);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RI

   /* Serial Interface (SI) */
   .mmio.si TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_SI : {
      tux64_platform_mips_n64_mmio_registers_si = .;
      *(.tux64.platform.mips.n64.mmio.si);
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_SI
}

