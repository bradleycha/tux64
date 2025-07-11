/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/interrupt.c - Interrupt service routine         */
/*    implementations.                                                        */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/interrupt.h"

void
tux64_boot_stage1_interrupt_initialize(void) {
   /*-------------------------------------------------------------------------*/
   /* TODO: implement, this is what we have to do:                            */
   /* 1. Assemble the following machine code to 0x80000180:                   */
   /*    j  tux64_boot_stage1_interrupt_service_routine                       */
   /*    nop                                                                  */
   /*                                                                         */
   /*    We don't store the code directly, as we need to ensure an absolute   */
   /*    jump is used.  We also need to memcpy it into place so we don't      */
   /*    have to further complicate stage-0.                                  */
   /*                                                                         */
   /* 2. Implement low-level handler.  This needs to store the register file  */
   /*    to a struct and replace "jr $ra" with "eret".  We need to write this */
   /*    in assembly so the compiler doesn't clobber registers.  We will also */
   /*    use a seperate stack to protect against possibly clobbering stack    */
   /*    variables when $sp is cleverly used.                                 */
   /*                                                                         */
   /* 3. Implement high-level handler.  This will check the interrupt source  */
   /*    and either parse it as an exception or a hardware interrupt, where   */
   /*    it will run its hardware-specific interrupt handler.                 */
   /*-------------------------------------------------------------------------*/
   return;
}

void
tux64_boot_stage1_interrupt_enable(void) {
   /* TODO: implement */
   return;
}

void
tux64_boot_stage1_interrupt_disable(void) {
   /* TODO: implement */
   return;
}

