/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/exec.h - Header for executing the kernel and stage-2.  */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_EXEC_H
#define _TUX64_BOOT_EXEC_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

struct Tux64BootExecKernelArgumentsFile {
   Tux64UInt32 address;
   Tux64UInt32 bytes;
};

struct Tux64BootExecKernelArguments {
   struct Tux64BootExecKernelArgumentsFile initramfs;
   struct Tux64BootExecKernelArgumentsFile command_line;
   Tux64UInt32 total_memory;
};

/*----------------------------------------------------------------------------*/
/* Starts the linux kernel given by the entrypoint.  Assumes interrupts are   */
/* disabled, the VI and AI are stopped, and there are no ongoing RSP, PI, or  */
/* SI DMA transfers.  'arguments' should be dynamically allocated and present */
/* after kernel load, or set to TUX64_NULLPTR if not present.                 */
/*----------------------------------------------------------------------------*/
void
tux64_boot_exec_kernel(
   const void * entrypoint,
   const struct Tux64BootExecKernelArguments * arguments
)
__attribute__((noreturn));

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_EXEC_H */

