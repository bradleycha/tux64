/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/interrupt.h - Interrupt service routine         */
/*    interface.                                                              */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_INTERRUPT_H
#define _TUX64_BOOT_STAGE1_INTERRUPT_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Initializes the interrupt service routines, setting up the handler and     */
/* initializing the processor state.                                          */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_interrupt_initialize(void);

void
tux64_boot_stage1_interrupt_enable(void);
void
tux64_boot_stage1_interrupt_disable(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_INTERRUPT_H */

