/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/status.c - Stage-1 bootloader status code       */
/*    implementations.                                                        */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/status.h"

#define TUX64_BOOT_STAGE1_STATUS_VALUE_HI\
   0x53544147 /* STAG */
#define TUX64_BOOT_STAGE1_STATUS_VALUE_LO\
   0x45313a00 /* E1:(null) */

struct Tux64BootStage1Status {
   Tux64UInt32 hi;
   Tux64UInt32 lo;
};

volatile struct Tux64BootStage1Status
tux64_boot_stage1_status
__attribute__((section(".status")));

void
tux64_boot_stage1_status_initialize(void) {
   tux64_boot_stage1_status.hi = TUX64_BOOT_STAGE1_STATUS_VALUE_HI;
   return;
}

void
tux64_boot_stage1_status_code_set(
   enum Tux64BootStage1StatusCode code
) {
   Tux64UInt32 word;

   word = TUX64_BOOT_STAGE1_STATUS_VALUE_LO;
   word = word | ((Tux64UInt32)code);

   tux64_boot_stage1_status.lo = word;

   return;
}

