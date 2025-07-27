/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/status.c - Stage-1 status code implementation.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/status.h"

#include <tux64/endian.h>

#define TUX64_BOOT_STAGE1_STATUS_MAGIC_HI\
   (0x53544147u) /* STAG */
#define TUX64_BOOT_STAGE1_STATUS_MAGIC_LO\
   (0x45313a00u) /* E1:(null) */

struct Tux64BootStage1Status {
   Tux64UInt32 hi;
   Tux64UInt32 lo;
};

static volatile struct Tux64BootStage1Status
tux64_boot_stage1_status
__attribute__((section(".status")));

void
tux64_boot_stage1_status_initialize(void) {
#if TUX64_BOOT_CONFIG_STATUS
   tux64_boot_stage1_status.hi = tux64_endian_convert_uint32(
      TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_STATUS_MAGIC_HI),
      TUX64_ENDIAN_FORMAT_BIG
   );
#else /* TUX64_BOOT_CONFIG_STATUS */
   (void)tux64_boot_stage1_status;
#endif /* TUX64_BOOT_CONFIG_STATUS */

   return;
}

void
tux64_boot_stage1_status_code_write(
   enum Tux64BootStage1StatusCode code
) {
   Tux64UInt32 lo;

#if TUX64_BOOT_CONFIG_STATUS
   lo = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_STATUS_MAGIC_LO);
   lo = lo | (Tux64UInt32)code;

   tux64_boot_stage1_status.lo = tux64_endian_convert_uint32(
      lo,
      TUX64_ENDIAN_FORMAT_BIG
   );
#else /* TUX64_BOOT_CONFIG_STATUS */
   (void)lo;
   (void)code;
#endif /* TUX64_BOOT_CONFIG_STATUS */

   return;
}

