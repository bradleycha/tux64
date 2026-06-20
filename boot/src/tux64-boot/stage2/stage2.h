/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.h - Stage-2 main entrypoint              */
/*    declarations.                                                           */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE2_H
#define _TUX64_BOOT_STAGE2_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/load.h"

/*----------------------------------------------------------------------------*/
/* The main function for the stage-2 bootloader.                              */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage2_main(
   Tux64UInt32 memory_total,
   Tux64BootLoadStatus load_status
)
__attribute__((noreturn));

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_H */

