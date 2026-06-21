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
#include "tux64-boot/load.h"

struct Tux64BootExecKernelArgumentsFile {
   Tux64UInt32 address;
   Tux64UInt32 bytes;
};

/*----------------------------------------------------------------------------*/
/* Boot arguments passed to the kernel.  Each primitive must be stored in the */
/* big-endian format.                                                         */
/*----------------------------------------------------------------------------*/
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
/* Starts the stage-2 bootloader, assuming it is already loaded into RSP      */
/* IMEM.  Assumes the boot header and allocations struct are loaded into the  */
/* appropriate locations in RSP DMEM.  Assumes interrupts are disabled, the   */
/* VI and AI are stopped, and there are no ongoing RSP, PI, or SI DMA         */
/* transfers.                                                                 */
/*----------------------------------------------------------------------------*/
void
tux64_boot_exec_stage2(
   Tux64UInt32 memory_total,
   Tux64BootLoadStatus load_status
)
__attribute__((noreturn));

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_EXEC_H */

