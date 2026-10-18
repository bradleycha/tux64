/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
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
/* initializing the processor state.  Interrupts remain disabled after        */
/* initialization until enabled manually.                                     */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_interrupt_initialize(void);

/*----------------------------------------------------------------------------*/
/* Enable or disable acknowledgement of interrupts from the CPU or external   */
/* sources.                                                                   */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_interrupt_enable(void);
void
tux64_boot_stage1_interrupt_disable(void);

/*----------------------------------------------------------------------------*/
/* Update the mask for the RCP's various interrupt sources.  This will enable */
/* receiving interrupts from the RCP based on the current mask.  By default,  */
/* all interrupt sources are masked.                                          */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_interrupt_rcp_update_mask(void);

/*----------------------------------------------------------------------------*/
/* CLear all pending interrupts originating from the RCP.                     */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_interrupt_rcp_clear(void);

/*----------------------------------------------------------------------------*/
/* Enable or disable various interrupts sent by the RCP.  Changes won't take  */
/* effect until calling tux64_boot_stage1_interrupt_rcp_update_mask().        */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_interrupt_vi_enable(void);
void
tux64_boot_stage1_interrupt_vi_disable(void);
void
tux64_boot_stage1_interrupt_ai_enable(void);
void
tux64_boot_stage1_interrupt_ai_disable(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_INTERRUPT_H */

