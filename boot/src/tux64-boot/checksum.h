/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/checksum.h - Header for specialized fletcher 64/32     */
/*    checksum algorithm.                                                     */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_CHECKSUM_H
#define _TUX64_BOOT_CHECKSUM_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

struct Tux64BootChecksumFletcher6432Context {
   Tux64UInt32 sum_hi;
   Tux64UInt32 sum_lo;
};

/*----------------------------------------------------------------------------*/
/* Initializes the fletcher 64/32 context.                                    */
/*----------------------------------------------------------------------------*/
void
tux64_boot_checksum_fletcher_64_32_initialize(
   struct Tux64BootChecksumFletcher6432Context * context
);

/*----------------------------------------------------------------------------*/
/* Digest bytes into the context.  'data' must be aligned on a 4-byte         */
/* boundary, and 'bytes' must be a multiple of 4 and non-zero.                */
/*----------------------------------------------------------------------------*/
void
tux64_boot_checksum_fletcher_64_32_digest(
   struct Tux64BootChecksumFletcher6432Context * context,
   const Tux64UInt8 * data,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Finalizes the fletcher 64/32 context, returning the digest value.          */
/*----------------------------------------------------------------------------*/
Tux64UInt32
tux64_boot_checksum_fletcher_64_32_finalize(
   struct Tux64BootChecksumFletcher6432Context * context
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_CHECKSUM_H */

