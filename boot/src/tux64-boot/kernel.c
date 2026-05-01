/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/kernel.c - Implementations for kernel booting.         */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/kernel.h"

/* since we want to maintain compatability with existing kernel loaders, such */
/* as libdragon's ELF loader and clbr's old n64bootloader, we pass a magic */
/* value for fw_arg1.  this allows the kernel to identify whether fw_arg0 */
/* contains a pointer to our arguments or not. */
#define TUX64_BOOT_KERNEL_MAGIC \
   (0x54583634u) /* TX64 */

typedef void (*Tux64BootKernelStart)(
   unsigned long fw_arg0,
   unsigned long fw_arg1,
   unsigned long fw_arg2,
   unsigned long fw_arg3
);

/* like in the stage-1 interrupt handler, gcc will complain if we directly */
/* pointer cast. */
union Tux64BootKernelStartPtr {
   Tux64BootKernelStart function;
   const void * data;
};

void
tux64_boot_kernel_start(
   const void * entrypoint,
   const struct Tux64BootKernelArguments * arguments
) {
   union Tux64BootKernelStartPtr kernel_start; 
   unsigned long fw_arg0;
   unsigned long fw_arg1;
   unsigned long fw_arg2;
   unsigned long fw_arg3;

   kernel_start.data = entrypoint;
   fw_arg0           = (unsigned long)(Tux64UIntPtr)arguments;
   fw_arg1           = TUX64_BOOT_KERNEL_MAGIC;
   fw_arg2           = 0u;
   fw_arg3           = 0u;

   kernel_start.function(fw_arg0, fw_arg1, fw_arg2, fw_arg3);
   TUX64_UNREACHABLE;
}

