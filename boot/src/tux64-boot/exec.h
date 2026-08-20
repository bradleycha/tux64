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

/*----------------------------------------------------------------------------*/
/* The location in memory of the kernel arguments struct.                     */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_EXEC_KERNEL_ARGUMENTS_ADDRESS\
   (TUX64_BOOT_LOAD_ALLOCATIONS_ADDRESS + TUX64_BOOT_LOAD_ALLOCATIONS_BYTES)

/*----------------------------------------------------------------------------*/
/* The size of the kernel arguments struct, in bytes.                         */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_EXEC_KERNEL_ARGUMENTS_BYTES\
   (0x10) /* TODO: generate this using AC_CHECK_SIZEOF(...) */

#if !TUX64_PREPROCESSOR_ONLY
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Boot arguments passed to the kernel.  Each primitive must be stored in the */
/* big-endian format.                                                         */
/*----------------------------------------------------------------------------*/
struct Tux64BootExecKernelArguments {
   Tux64UInt32 initramfs_address;
   Tux64UInt32 initramfs_bytes;
   Tux64UInt32 command_line_address;
   Tux64UInt32 total_memory;
};

/*----------------------------------------------------------------------------*/
/* Initializes the kernel arguments struct.                                   */
/*----------------------------------------------------------------------------*/
void
tux64_boot_exec_kernel_arguments_initialize(
   Tux64UInt32 initramfs_address,
   Tux64UInt32 initramfs_bytes,
   Tux64UInt32 command_line_address,
   Tux64UInt32 total_memory
);

/*----------------------------------------------------------------------------*/
/* Starts the linux kernel given by the entrypoint.  Assumes interrupts are   */
/* disabled, the VI and AI are stopped, and there are no ongoing RSP, PI, or  */
/* SI DMA transfers.  Kernel arguments must first be initialized using        */
/* tux64_boot_exec_kernel_arguments_initialize() before executing.            */
/*----------------------------------------------------------------------------*/
void
tux64_boot_exec_kernel(
   const void * entrypoint
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
   Tux64BootLoadStatus load_status
)
__attribute__((noreturn));

/*----------------------------------------------------------------------------*/
#endif /* !TUX64_PREPROCESSOR_ONLY */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_EXEC_H */

