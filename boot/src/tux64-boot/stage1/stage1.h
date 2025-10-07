/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.h - Stage-1 main entrypoint              */
/*    declarations.                                                           */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_H
#define _TUX64_BOOT_STAGE1_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/ipl2.h"

/*----------------------------------------------------------------------------*/
/* The main function for the stage-1 bootloader.                              */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_main(
   enum Tux64BootIpl2RomType rom_type,
   enum Tux64BootIpl2VideoStandard video_standard,
   enum Tux64BootIpl2ResetType reset_type,
   Tux64UInt8 rom_cic_seed,
   Tux64UInt8 pif_rom_version,
   Tux64UInt32 memory_total,
   Tux64UInt32 memory_available,
   Tux64Boolean running_on_ique
)
__attribute__((noreturn));

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_H */

