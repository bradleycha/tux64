/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/preempt.h - Header for preemption management.   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_PREEMPT_H
#define _TUX64_BOOT_STAGE1_PREEMPT_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Returns if preemption is requested, and the calling code should be         */
/* interrupted.  This is useful for CPU-intensive calculation loops, and      */
/* yielding to I/O devies.                                                    */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_boot_stage1_preempt_yield(void);

/*----------------------------------------------------------------------------*/
/* Idle the CPU until a preemption request is made.                           */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_preempt_wait(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_PREEMPT_H */

