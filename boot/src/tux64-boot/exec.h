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
#include <tux64/endian.h>
#include "tux64-boot/ipl2.h"
#include "tux64-boot/load.h"

/*----------------------------------------------------------------------------*/
/* Boot arguments passed to the kernel.  Each primitive must be stored in the */
/* big-endian format.                                                         */
/*----------------------------------------------------------------------------*/
struct Tux64BootExecKernelArguments {
   Tux64UInt32 initramfs_address;
   Tux64UInt32 initramfs_bytes;
   Tux64UInt32 rootfs_cart_address;
   Tux64UInt32 rootfs_bytes;
   Tux64UInt32 command_line_address;
   Tux64UInt32 total_memory;
   Tux64UInt32 rng_seed;
   Tux64UInt32 metadata;
};

struct Tux64BootExecKernelMetadataIpl2 {
   enum Tux64BootIpl2RomType rom_type;
   enum Tux64BootIpl2ResetType reset_type;
   Tux64UInt8 rom_cic_seed;
   Tux64UInt8 pif_rom_version;
};

/*----------------------------------------------------------------------------*/
/* Bootloader and hardware metadata intended primarily for informative and    */
/* diagnostic purposes.                                                       */
/*----------------------------------------------------------------------------*/
struct Tux64BootExecKernelMetadata {
   enum Tux64BootConsoleType console_type;
   struct Tux64BootExecKernelMetadataIpl2 ipl2;
};

/*----------------------------------------------------------------------------*/
/* Initializes the kernel arguments struct.                                   */
/*----------------------------------------------------------------------------*/
void
tux64_boot_exec_kernel_arguments_initialize(
   Tux64UInt32 initramfs_address,
   Tux64UInt32 initramfs_bytes,
   Tux64UInt32 rootfs_cart_address,
   Tux64UInt32 rootfs_bytes,
   Tux64UInt32 command_line_address,
   Tux64UInt32 total_memory,
   Tux64UInt32 rng_seed,
   const struct Tux64BootExecKernelMetadata * metadata
);

/*----------------------------------------------------------------------------*/
/* Starts the linux kernel given by the entrypoint.  Assumes interrupts are   */
/* disabled, the VI and AI are stopped, and there are no ongoing RSP, PI, or  */
/* SI DMA transfers.  Kernel arguments must first be initialized using        */
/* tux64_boot_exec_kernel_arguments_initialize() before executing.  If        */
/* TUX64_BOOT_CONFIG_FOREIGN_ENDIAN_KERNELS is disabled, it is assumed that   */
/* endian_format == TUX64_ENDIAN_FORMAT_NATIVE.  Otherwise, endian switching  */
/* is done before executing the kernel.                                       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_exec_kernel(
   const void * entrypoint,
   enum Tux64EndianFormat endian_format
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
#endif /* _TUX64_BOOT_EXEC_H */

