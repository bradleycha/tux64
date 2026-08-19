/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/allocations.h - Memory definitions for stage-2  */
/*    load allocations struct.                                                */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE2_ALLOCATIONS_H
#define _TUX64_BOOT_STAGE2_ALLOCATIONS_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/memory-map.h>

/*----------------------------------------------------------------------------*/
/* The base address for the load allocations in RSP DMEM.                     */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STAGE2_ALLOCATIONS_ADDRESS\
   TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM
   /* placed at at the start of RSP DMEM for convenience */

#if !TUX64_PREPROCESSOR_ONLY
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* The globally accessible load allocations struct from stage-1.              */
/*----------------------------------------------------------------------------*/
extern const volatile struct Tux64BootLoadAllocations
tux64_boot_stage2_allocations
__attribute((section(".allocations")));

/*----------------------------------------------------------------------------*/
#endif /* !TUX64_PREPROCESSOR_ONLY */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_ALLOCATIONS_H */

