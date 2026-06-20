/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/exec.c - Implementations for starting the kernel and   */
/*    stage-2.                                                                */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/exec.h"

/* since we want to maintain compatability with existing kernel loaders, such */
/* as libdragon's ELF loader and clbr's old n64bootloader, we pass a magic */
/* value for fw_arg1.  this allows the kernel to identify whether fw_arg0 */
/* contains a pointer to our arguments or not. */
#define TUX64_BOOT_EXEC_KERNEL_MAGIC \
   (0x54583634u) /* TX64 */

typedef void (*Tux64BootExecStartFunction)();

/* like in the stage-1 interrupt handler, gcc will complain if we directly */
/* pointer cast. */
union Tux64BootExecStartPtr {
   Tux64BootExecStartFunction function;
   const void * data;
};

void
tux64_boot_exec_kernel(
   const void * entrypoint,
   const struct Tux64BootExecKernelArguments * arguments
) {
   /* we're manually specifying the registers instead of adding function */
   /* arguments to ensure a stable ABI. */
   union Tux64BootExecStartPtr start; 
   register unsigned long fw_arg0 __asm__("$a0");
   register unsigned long fw_arg1 __asm__("$a1");
   register unsigned long fw_arg2 __asm__("$a2");
   register unsigned long fw_arg3 __asm__("$a3");

   start.data  = entrypoint;
   fw_arg0     = (unsigned long)(Tux64UIntPtr)arguments;
   fw_arg1     = TUX64_BOOT_EXEC_KERNEL_MAGIC;
   fw_arg2     = 0u;
   fw_arg3     = 0u;

   TUX64_EXPLICIT_DEPENDENCY(fw_arg0);
   TUX64_EXPLICIT_DEPENDENCY(fw_arg1);
   TUX64_EXPLICIT_DEPENDENCY(fw_arg2);
   TUX64_EXPLICIT_DEPENDENCY(fw_arg3);
   start.function();
   TUX64_UNREACHABLE;
}

