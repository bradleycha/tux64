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

struct Tux64BootStage1InterruptRegisterFile {
   union Tux64IntPtr zero;
   union Tux64IntPtr at;
   union Tux64IntPtr v0;
   union Tux64IntPtr v1;
   union Tux64IntPtr a0;
   union Tux64IntPtr a1;
   union Tux64IntPtr a2;
   union Tux64IntPtr a3;
   union Tux64IntPtr t0;
   union Tux64IntPtr t1;
   union Tux64IntPtr t2;
   union Tux64IntPtr t3;
   union Tux64IntPtr t4;
   union Tux64IntPtr t5;
   union Tux64IntPtr t6;
   union Tux64IntPtr t7;
   union Tux64IntPtr s0;
   union Tux64IntPtr s1;
   union Tux64IntPtr s2;
   union Tux64IntPtr s3;
   union Tux64IntPtr s4;
   union Tux64IntPtr s5;
   union Tux64IntPtr s6;
   union Tux64IntPtr s7;
   union Tux64IntPtr t8;
   union Tux64IntPtr t9;
   union Tux64IntPtr k0;
   union Tux64IntPtr k1;
   union Tux64IntPtr gp;
   union Tux64IntPtr sp;
   union Tux64IntPtr fp;
   union Tux64IntPtr ra;
   union Tux64IntPtr pc;
   union Tux64IntPtr lo;
   union Tux64IntPtr hi;
};

/*----------------------------------------------------------------------------*/
/* The CPU's state coming from an interrupt.                                  */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1InterruptContext {
   struct Tux64BootStage1InterruptRegisterFile register_file;
};

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

