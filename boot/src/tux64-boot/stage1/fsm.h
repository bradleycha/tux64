/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/fsm.h - Header for main finite state machine.   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_FSM_H
#define _TUX64_BOOT_STAGE1_FSM_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/pi.h>
#include <tux64/checksum.h>
#include "tux64-boot/load.h"
#include "tux64-boot/stage1/fbcon.h"
#include "tux64-boot/stage1/format.h"

struct Tux64BootStage1Fsm;

typedef void (*Tux64BootStage1FsmPfnState)(struct Tux64BootStage1Fsm * fsm);
typedef void (*Tux64BootStage1FsmPfnTransition)(struct Tux64BootStage1Fsm * fsm);

struct Tux64BootStage1FsmMemoryDelay {
   Tux64BootStage1FsmPfnTransition transition;
   Tux64UInt32 ticks_remaining;
};

struct Tux64BootStage1FsmMemoryLoadFile {
   struct Tux64BootStage1FormatPercentageContext percentage_progress;
   Tux64BootStage1FbconLabel label_percentage_progress;
   Tux64BootStage1FsmPfnTransition transition_next;
   Tux64UInt32 iter_addr_rdram;
   Tux64PlatformMipsN64PiBusAddress iter_addr_cart;
   Tux64UInt32 bytes_remaining;
   union Tux64Int32 checksum_expected;
   struct Tux64ChecksumContext checksum_context;
};

struct Tux64BootStage1FsmMemoryTest {
   struct Tux64BootStage1FormatPercentageContext percentage;
   Tux64BootStage1FbconLabel label;
};

union Tux64BootStage1FsmMemory {
   struct Tux64BootStage1FsmMemoryDelay delay;
   struct Tux64BootStage1FsmMemoryLoadFile load_file;
   struct Tux64BootStage1FsmMemoryTest test;
};

struct Tux64BootStage1FsmGlobalsLoadInfo {
   struct Tux64BootLoadAllocations allocations;
   Tux64BootLoadStatus status;
};

struct Tux64BootStage1FsmGlobalsStage2 {
   /* aligned for both PI dma and RSP dma. */
   Tux64UInt8 dma_buffer [4096u]
   __attribute__((aligned(8u)));
};

struct Tux64BootStage1FsmGlobals {
   struct Tux64BootStage1FsmGlobalsLoadInfo load_info;
   struct Tux64BootStage1FsmGlobalsStage2 stage2;
};

struct Tux64BootStage1Fsm {
   Tux64BootStage1FsmPfnState state;
   union Tux64BootStage1FsmMemory memory;
   struct Tux64BootStage1FsmGlobals globals;
};

/*----------------------------------------------------------------------------*/
/* Initializes the finite state machine.                                      */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_fsm_initialize(
   struct Tux64BootStage1Fsm * fsm
);

/*----------------------------------------------------------------------------*/
/* Executes one tick of the finite state machine.                             */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_fsm_execute(
   struct Tux64BootStage1Fsm * fsm
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_FSM_H */

