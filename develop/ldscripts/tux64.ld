/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* develop/ldscripts/tux64.ld - Main linker script used to define external    */
/*    MMIO, variables, and setup for external projects.                       */
/*----------------------------------------------------------------------------*/

INCLUDE tux64/platform_n64/memory_map.ld
INCLUDE tux64/platform_n64/mi.ld
INCLUDE tux64/platform_n64/vi.ld
INCLUDE tux64/platform_n64/ai.ld
INCLUDE tux64/platform_n64/pi.ld
INCLUDE tux64/platform_n64/ri.ld
INCLUDE tux64/platform_n64/si.ld

