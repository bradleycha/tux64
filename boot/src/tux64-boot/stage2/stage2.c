/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage2/stage2.c - Stage-2 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

#include "tux64-boot/initialize.h"
#include "tux64-boot/header.h"
#include "tux64-boot/exec.h"
#include "tux64-boot/stage2/status.h"
#include "tux64-boot/stage2/allocations.h"
#include "tux64-boot/stage2/file.h"

__attribute__((noreturn))
static void
tux64_boot_stage2_start_kernel(
   Tux64UInt32 memory_total
) {
   Tux64UInt32 addr_arguments;
   struct Tux64BootExecKernelArguments * arguments;

   addr_arguments = tux64_boot_stage2_allocations.required.kernel_args.address;
   arguments      = (struct Tux64BootExecKernelArguments *)(Tux64UIntPtr)addr_arguments;

   /* while we could initialize this from stage-1 if allocation succeeded, we */
   /* would then need a conditional and include the init code anyways.  it's */
   /* cheaper, in terms of code size, to just always initialize the kernel */
   /* arguments. */
   tux64_boot_exec_kernel_arguments_initialize(
      arguments,
      tux64_boot_stage2_allocations.optional.initramfs.address,
      tux64_boot_header_file_initramfs()->length,
      tux64_boot_stage2_allocations.optional.command_line.address,
      memory_total
   );

   tux64_boot_exec_kernel(
      (const void *)tux64_boot_header_file_kernel()->addr_entry,
      arguments
   );
}

void
tux64_boot_stage2_main(
   Tux64UInt32 memory_total,
   Tux64BootLoadStatus load_status
) {
   tux64_boot_initialize();

   tux64_boot_stage2_status_code_write(TUX64_BOOT_STAGE2_STATUS_CODE_BEGIN);

   tux64_boot_stage2_file_load_on_demand(load_status);

   tux64_boot_stage2_status_code_write(TUX64_BOOT_STAGE2_STATUS_CODE_START_KERNEL);

   tux64_boot_stage2_start_kernel(memory_total);
   TUX64_UNREACHABLE;
}

