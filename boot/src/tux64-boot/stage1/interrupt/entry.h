/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/interrupt/entry.h - Interrupt service routine   */
/*    entry function declaractions.                                           */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_INTERRUPT_ENTRY_H
#define _TUX64_BOOT_STAGE1_INTERRUPT_ENTRY_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* The low-level interrupt service entry function which saves and restores    */
/* the CPU context.                                                           */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_interrupt_entry(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_INTERRUPT_ENTRY_H */

