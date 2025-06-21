/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/stage1.c - Stage-1 bootloader main entrypoint.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/status.h"

/* BIG FAT TODO: we need to configure the compiler to not use non 32-bit      */
/* reads and writes.  This is because of the hardware quirk mentioned in      */
/* stage0, but now we don't have precise control over the machine code.       */
/* We coooooould hack out 64-bit memory access by using -mabi=o32, but that   */
/* is shit and doesn't solve the root problem.  We also could try to avoid    */
/* data structures which are non 32-bit, but now we can't reliably use        */
/* library functions.  Basically, we might be fucked and have to resort to    */
/* writing this all in assembly...hopefully not!                              */

void
tux64_boot_stage1_start(void)
__attribute__((noreturn, section(".start")));

void
tux64_boot_stage1_start(void) {
   tux64_boot_stage1_status_initialize();
   tux64_boot_stage1_status_code_set(TUX64_BOOT_STAGE1_STATUS_CODE_BEGIN);

   /* TODO: implement */
   TUX64_HALT;
}

