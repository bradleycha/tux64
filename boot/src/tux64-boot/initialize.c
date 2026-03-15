/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/initialize.c - Implementations for common              */
/*    initialization routines.                                                */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/initialize.h"

#include "tux64-boot/builtin.h"
#include "tux64-boot/gp.h"
#include "tux64-boot/status.h"

void
tux64_boot_initialize(void) {
   /* it's important for tux64_boot_builtin_link() to be first since it uses  */
   /* volatile inline asm, we don't want to create a false dependency for     */
   /* the rest of the init code.                                              */
   tux64_boot_builtin_link();
   tux64_boot_gp_load();
   tux64_boot_status_code_initialize();
   return;
}

