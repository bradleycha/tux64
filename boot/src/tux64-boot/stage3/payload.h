/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage3/payload.h - Header for the stage-3 bootloader   */
/*    payload passed from the stage-2 bootloader.                             */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE3_PAYLOAD_H
#define _TUX64_BOOT_STAGE3_PAYLOAD_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

struct Tux64BootStage3PayloadItemLengths {
   Tux64UInt32 kernel_data;
   Tux64UInt32 kernel_bss;
   Tux64UInt32 initramfs;
   Tux64UInt32 command_line;
};

/* the information passed from stage-2 to stage-3, stored at the start of RSP */
/* DMEM before .bss */
/* WARNING: the size of this struct must be defined in stage3/payload.ld in */
/* order to define the location of .bss afterwards.  if you modify this */
/* struct, make sure to update its size in stage3/payload.ld! */
struct Tux64BootStage3Payload {
   struct Tux64BootStage3PayloadItemLengths item_lengths;
};

/* this is defined in the linker script, so don't think too much about it. */
/* we don't need to use volatile here because it's just normal memory only */
/* being written to/read from a single thread. */
extern struct Tux64BootStage3Payload
tux64_boot_stage3_payload
__attribute__((section(".stage3.payload")));

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE3_PAYLOAD_H */

