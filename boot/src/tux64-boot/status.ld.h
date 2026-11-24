/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/status.ld.h - Boot status code linker script           */
/*    definitions.                                                            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STATUS_LD_H
#define _TUX64_BOOT_STATUS_LD_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.ld.h"
#include "tux64-boot/layout.h"

SECTIONS
{
   .rsp_dmem.status (TUX64_BOOT_LAYOUT_STATUS_ADDRESS) : {
      tux64_boot_status_code = .;
   } :noload_rsp_dmem =0x00
}

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STATUS_LD_H */

