/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/kernel.h - Header for kernel booting.                  */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_KERNEL_H
#define _TUX64_BOOT_KERNEL_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

struct Tux64BootKernelArgumentsFile {
   Tux64UInt32 address;
   Tux64UInt32 bytes;
};

struct Tux64BootKernelArguments {
   struct Tux64BootKernelArgumentsFile initramfs;
   struct Tux64BootKernelArgumentsFile command_line;
   Tux64UInt32 total_memory;
};

/*----------------------------------------------------------------------------*/
/* Starts the linux kernel given by the entrypoint.  Assumes interrupts are   */
/* disabled, the VI and AI are stopped, and there are no ongoing RSP, PI, or  */
/* SI DMA transfers.  'arguments' should be dynamically allocated and present */
/* after kernel load, or set to TUX64_NULLPTR if not present.                 */
/*----------------------------------------------------------------------------*/
void
tux64_boot_kernel_start(
   const void * entrypoint,
   const struct Tux64BootKernelArguments * arguments
)
__attribute__((noreturn));

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_KERNEL_H */

