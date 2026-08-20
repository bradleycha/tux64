/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/exec.ld.h - Linker script definitions for kernel       */
/*    arguments struct.                                                       */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_EXEC_LD_H
#define _TUX64_BOOT_EXEC_LD_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"
#include "tux64-boot/layout.h"

SECTIONS
{
   .rdram.kernel_arguments (TUX64_BOOT_LAYOUT_EXEC_KERNEL_ARGUMENTS_ADDRESS) : {
      tux64_boot_exec_kernel_arguments = .;

      . += TUX64_BOOT_LAYOUT_EXEC_KERNEL_ARGUMENTS_BYTES;
   } >TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_LAYOUT_RDRAM_CACHED =0x00
}

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_EXEC_LD_H */

