/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.c - Stage-2 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

#include "tux64-boot/initialize.h"
#include "tux64-boot/header.h"
#include "tux64-boot/load.h"
#include "tux64-boot/exec.h"
#include "tux64-boot/stage2/status.h"
#include "tux64-boot/stage2/file.h"

__attribute__((noreturn))
static void
tux64_boot_stage2_start_kernel(void) {
   const void * entrypoint;

   entrypoint = (const void *)tux64_boot_header_file_kernel()->addr_entry;

   tux64_boot_exec_kernel(entrypoint);
}

void
tux64_boot_stage2_main(
   Tux64BootLoadStatus load_status
) {
   tux64_boot_initialize();

   tux64_boot_stage2_status_code_write(TUX64_BOOT_STAGE2_STATUS_CODE_BEGIN);

   tux64_boot_stage2_file_load_on_demand(load_status);

   tux64_boot_stage2_status_code_write(TUX64_BOOT_STAGE2_STATUS_CODE_START_KERNEL);

   tux64_boot_stage2_start_kernel();
   TUX64_UNREACHABLE;
}

