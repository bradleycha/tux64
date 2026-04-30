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
#include "tux64-boot/stage1/fbcon.h"
#include "tux64-boot/stage1/format.h"

struct Tux64BootStage1Fsm;

typedef void (*Tux64BootStage1FsmPfnState)(struct Tux64BootStage1Fsm * fsm);
typedef void (*Tux64BootStage1FsmPfnTransition)(struct Tux64BootStage1Fsm * fsm);

struct Tux64BootStage1FsmMemoryDelay {
   Tux64BootStage1FsmPfnTransition transition;
   Tux64UInt32 ticks_remaining;
};

struct Tux64BootStage1FsmMemoryTest {
   struct Tux64BootStage1FormatPercentageContext percentage;
   Tux64BootStage1FbconLabel label;
};

union Tux64BootStage1FsmMemory {
   struct Tux64BootStage1FsmMemoryDelay delay;
   struct Tux64BootStage1FsmMemoryTest test;
};

struct Tux64BootStage1Fsm {
   union Tux64BootStage1FsmMemory memory;
   Tux64BootStage1FsmPfnState state;
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

