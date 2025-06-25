/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/checksum.c - Implementations for checksum functions.         */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/checksum.h"

static void
tux64_checksum_fletcher_64_32_initialize(
   struct Tux64ChecksumContext * context
) {
   /* TODO: implement */
   (void)context;
   return;
}

static void
tux64_checksum_fletcher_64_32_digest(
   struct Tux64ChecksumContext * context,
   const Tux64UInt8 * data,
   Tux64UInt32 bytes
) {
   /* TODO: implement */
   (void)context;
   (void)data;
   (void)bytes;
   return;
}

static Tux64UInt8 *
tux64_checksum_fletcher_64_32_finalize(
   struct Tux64ChecksumContext * context
) {
   /* TODO: implement */
   (void)context;
   return TUX64_NULLPTR;
}

const struct Tux64ChecksumFunction
tux64_checksum_fletcher_64_32 = {
   .initialize    = tux64_checksum_fletcher_64_32_initialize,
   .digest        = tux64_checksum_fletcher_64_32_digest,
   .finalize      = tux64_checksum_fletcher_64_32_finalize,
   .digest_bytes  = TUX64_LITERAL_UINT8(TUX64_CHECKSUM_FLETCHER_64_32_DIGEST_BYTES)
};

