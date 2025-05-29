/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/n64.ld - Project-wide N64 linker script configuration.            */
/*----------------------------------------------------------------------------*/

/* TODO: move this file into a seperate project dir which is installed to the */
/* ${TUX64_BUILD_ROOT}/tools/ directory. */

/* different virtual address segments */
N64_ADDRESS_SEGMENT_DIRECT_CACHED   = 0x80000000;
N64_ADDRESS_SEGMENT_DIRECT_UNCACHED = 0xa0000000;

/* physical address of various sections of memory */
N64_ADDRESS_PHYSICAL_RDRAM    = 0x00000000;
N64_ADDRESS_PHYSICAL_RSP_DMEM = 0x04000000;
N64_ADDRESS_PHYSICAL_RSP_IMEM = 0x04001000;

/* virtual addresses of relevant sections of physical memory */
/* note that only RDRAM supports cached access.  attempting to access any */
/* other physical segments as cached will hang the CPU */
N64_ADDRESS_VIRTUAL_RDRAM_UNCACHED =
   N64_ADDRESS_SEGMENT_DIRECT_UNCACHED + N64_ADDRESS_PHYSICAL_RDRAM;
N64_ADDRESS_VIRTUAL_RDRAM_CACHED =
   N64_ADDRESS_SEGMENT_DIRECT_CACHED + N64_ADDRESS_PHYSICAL_RDRAM;
N64_ADDRESS_VIRTUAL_RSP_DMEM =
   N64_ADDRESS_SEGMENT_DIRECT_UNCACHED + N64_ADDRESS_PHYSICAL_RSP_DMEM;
N64_ADDRESS_VIRTUAL_RSP_IMEM =
   N64_ADDRESS_SEGMENT_DIRECT_UNCACHED + N64_ADDRESS_PHYSICAL_RSP_IMEM;

/* memory regions directly accessible by the CPU */
MEMORY
{
   N64_MEMORY_RDRAM_UNCACHED (xrw)
      : ORIGIN = N64_ADDRESS_VIRTUAL_RDRAM_UNCACHED
      LENGTH = 0x03f00000

   N64_MEMORY_RDRAM_CACHED (xrw)
      : ORIGIN = N64_ADDRESS_VIRTUAL_RDRAM_CACHED,
      LENGTH = 0x03f00000

   N64_MEMORY_RSP_DMEM (xrw)
      : ORIGIN = N64_ADDRESS_VIRTUAL_RSP_DMEM,
      LENGTH = 0x1000

   N64_MEMORY_RSP_IMEM (xrw)
      : ORIGIN = N64_ADDRESS_VIRTUAL_RSP_IMEM,
      LENGTH = 0x1000
}

