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

/* we can assume we're using 64-bit integers since we always target VR4300 */
typedef union Tux64Int64 Tux64BootStage1InterruptContextRegister;

struct Tux64BootStage1InterruptContextRegisterFile {
   /* warning: if you change the order of these, make sure to update the asm */
   /* entry function as well.  i tried using offsetof with everything, but */
   /* for some reason GCC doesn't let me use more than 30 operands... */
   /* also I know it's a bit goofy to store $zero, but it may be useful for */
   /* debugging purposes. */
   Tux64BootStage1InterruptContextRegister zero;
   Tux64BootStage1InterruptContextRegister at;
   Tux64BootStage1InterruptContextRegister v0;
   Tux64BootStage1InterruptContextRegister v1;
   Tux64BootStage1InterruptContextRegister a0;
   Tux64BootStage1InterruptContextRegister a1;
   Tux64BootStage1InterruptContextRegister a2;
   Tux64BootStage1InterruptContextRegister a3;
   Tux64BootStage1InterruptContextRegister t0;
   Tux64BootStage1InterruptContextRegister t1;
   Tux64BootStage1InterruptContextRegister t2;
   Tux64BootStage1InterruptContextRegister t3;
   Tux64BootStage1InterruptContextRegister t4;
   Tux64BootStage1InterruptContextRegister t5;
   Tux64BootStage1InterruptContextRegister t6;
   Tux64BootStage1InterruptContextRegister t7;
   Tux64BootStage1InterruptContextRegister s0;
   Tux64BootStage1InterruptContextRegister s1;
   Tux64BootStage1InterruptContextRegister s2;
   Tux64BootStage1InterruptContextRegister s3;
   Tux64BootStage1InterruptContextRegister s4;
   Tux64BootStage1InterruptContextRegister s5;
   Tux64BootStage1InterruptContextRegister s6;
   Tux64BootStage1InterruptContextRegister s7;
   Tux64BootStage1InterruptContextRegister t8;
   Tux64BootStage1InterruptContextRegister t9;
   Tux64BootStage1InterruptContextRegister k0;
   Tux64BootStage1InterruptContextRegister k1;
   Tux64BootStage1InterruptContextRegister gp;
   Tux64BootStage1InterruptContextRegister sp;
   Tux64BootStage1InterruptContextRegister fp;
   Tux64BootStage1InterruptContextRegister ra;
   Tux64BootStage1InterruptContextRegister pc;
   Tux64BootStage1InterruptContextRegister lo;
   Tux64BootStage1InterruptContextRegister hi;
};

/*----------------------------------------------------------------------------*/
/* The CPU's state coming from an interrupt.                                  */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1InterruptContext {
   struct Tux64BootStage1InterruptContextRegisterFile register_file;
};

/*----------------------------------------------------------------------------*/
/* Initializes the interrupt service routines, setting up the handler and     */
/* initializing the processor state.                                          */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_interrupt_initialize(void);

/*----------------------------------------------------------------------------*/
/* Enable or disable VI interrupts.                                           */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_interrupt_vi_enable(void);
void
tux64_boot_stage1_interrupt_vi_disable(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_INTERRUPT_H */

