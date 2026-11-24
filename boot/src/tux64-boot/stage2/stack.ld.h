/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stack.ld.h - Stage-2 stack linker script        */
/*    definitions.                                                            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE2_STACK_LD_H
#define _TUX64_BOOT_STAGE2_STACK_LD_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"
#include "tux64-boot/layout.h"

SECTIONS
{
   .rdram.stack (TUX64_BOOT_LAYOUT_STAGE2_STACK_ADDRESS) : {
      . += TUX64_BOOT_LAYOUT_STAGE2_STACK_BYTES;
   } :noload_rdram_cached =0x00
}

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE2_STACK_LD_H */

