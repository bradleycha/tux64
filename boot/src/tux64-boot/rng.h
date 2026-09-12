/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/rng.h - Header for random number generation.           */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_RNG_H
#define _TUX64_BOOT_RNG_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Initializes the random number generator using the given seed for entropy.  */
/*----------------------------------------------------------------------------*/
void
tux64_boot_rng_initialize(
   Tux64UInt32 seed
);

/*----------------------------------------------------------------------------*/
/* Generates a random integer.                                                */
/*----------------------------------------------------------------------------*/
Tux64UInt8
tux64_boot_rng_random_uint8(void);
Tux64UInt16
tux64_boot_rng_random_uint16(void);
Tux64UInt32
tux64_boot_rng_random_uint32(void);
Tux64UInt64
tux64_boot_rng_random_uint64(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_RNG_H */

