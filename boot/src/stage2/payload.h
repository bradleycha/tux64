/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/stage2/payload.h - Header for the stage-2 bootloader payload      */
/*    passed from the stage-1 bootloader.                                     */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE2_PAYLOAD_H
#define _TUX64_BOOT_STAGE2_PAYLOAD_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot.h"

struct Tux64BootStage2PayloadItemLengths {
   Tux64UInt32 kernel_data;
   Tux64UInt32 kernel_bss;
   Tux64UInt32 initramfs;
   Tux64UInt32 command_line;
};

/* the information passed from stage-1 to stage-2, stored at the start of RSP */
/* DMEM before .bss */
/* WARNING: the size of this struct must be defined in stage2/payload.ld in */
/* order to define the location of .bss afterwards.  if you modify this */
/* struct, make sure to update its size in stage2/payload.ld! */
struct Tux64BootStage2Payload {
   struct Tux64BootStage2PayloadItemLengths item_lengths;
};

/* this is defined in the linker script, so don't think too much about it. */
/* we don't need to use volatile here because it's just normal memory only */
/* being written to/read from a single thread. */
extern struct Tux64BootStage2Payload
tux64_boot_stage2_payload
__attribute__((section(".stage2.payload")));

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_PAYLOAD_H */

