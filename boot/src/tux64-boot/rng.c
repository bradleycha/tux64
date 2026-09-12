/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/rng.c - Implementations for random number generation.  */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/rng.h"

#include <tux64/endian.h>

/*----------------------------------------------------------------------------*/
/* this random number generator is very simple computationally, but requires  */
/* a little group theory.  we take integers mod p, where 'p' is prime, and    */
/* let it be a group without zero under modular multiplication.  we then wish */
/* to find a generator for Z_p under multiplication.  this is desirable       */
/* because if f(n) = a^n, where 'a' is our generator for Z_p, then f is a     */
/* bijection.  therefore, every number in Z_p is reachable and doesn't repeat */
/* until every other number in Z_p has been computed.                         */
/*                                                                            */
/* we now apply the above knowledge to get the chosen parameters below.  we   */
/* have that the closest prime to 2^32 is 2^32 - 5, which lets us reach all   */
/* but 6 values in a 32-bit integer (we don't allow zero since it breaks our  */
/* generator).  we then pick 2^31 + 2^30 - 11 as our generator.  this is      */
/* chosen as it's pretty large, so spreads out multiplication evenly.  this   */
/* generator was found by first calculating (2**32 - 5) * 3/4, then finding   */
/* the nearest prime which turned out to be a generator.  checking if a       */
/* prime is a generator for our group can be found on google.                 */
/*                                                                            */
/* note that both zero and TUX64_UINT32_MAX are unreachable using this        */
/* approach.  that's why we add an offset to the return value.  internally,   */
/* we are only in Z_p under multiplication, however we add 2^20 to shift the  */
/* unreachable values to an insignificant range.  that is, it's better to     */
/* have [2^20, 2^20 + 5] be unreachable than zero and TUX64_UINT32_MAX.       */
/* there is otherwise no specific reason this number is chosen specifically.  */
/*                                                                            */
/* also note that this is not cryptographically secure what-so-ever.  in      */
/* fact, it's trivial to reverse the RNG state from the output, given both    */
/* addition and multiplicative inverses are trivial to compute since we are   */
/* multiplying under prime-modular arithmetic (O(log n)-computable).          */
/* however, this is fine since we are only using this for non-cryptographic   */
/* applications and seeding the kernel's CRNG.                                */
/*----------------------------------------------------------------------------*/

#define TUX64_BOOT_RNG_GROUP_MODULUS\
   (0xfffffffbu) /* 2^32 - 5 */
#define TUX64_BOOT_RNG_GROUP_GENERATOR\
   (0xbffffff5u) /* 2^31 + 2^30 - 11 */
#define TUX64_BOOT_RNG_GROUP_OFFSET\
   (0x00100000u) /* 2^20 */

struct Tux64BootRngState {
   Tux64UInt32 current;
};

static struct Tux64BootRngState
tux64_boot_rng_state;

void
tux64_boot_rng_initialize(
   Tux64UInt32 seed
) {
   /* make sure the seed is within our group so we maintain closure. */
   seed = (seed % TUX64_LITERAL_UINT32(TUX64_BOOT_RNG_GROUP_MODULUS));
   if (seed == TUX64_LITERAL_UINT32(0u)) {
      seed = TUX64_LITERAL_UINT32(1u);
   }

   tux64_boot_rng_state.current = seed;
   return;
}

Tux64UInt8
tux64_boot_rng_random_uint8(void) {
   return (Tux64UInt8)tux64_boot_rng_random_uint32();
}

Tux64UInt16
tux64_boot_rng_random_uint16(void) {
   return (Tux64UInt16)tux64_boot_rng_random_uint32();
}

Tux64UInt32
tux64_boot_rng_random_uint32(void) {
   Tux64UInt32 value_current;
   Tux64UInt32 value_next;
   Tux64UInt32 retn;

   value_current = tux64_boot_rng_state.current;

   value_next = value_current;
   value_next = (value_next * TUX64_LITERAL_UINT32(TUX64_BOOT_RNG_GROUP_GENERATOR));
   value_next = (value_next % TUX64_LITERAL_UINT32(TUX64_BOOT_RNG_GROUP_MODULUS));

   tux64_boot_rng_state.current = value_next;

   retn = value_current;
   retn = (retn + TUX64_LITERAL_UINT32(TUX64_BOOT_RNG_GROUP_OFFSET));
   return retn;
}

Tux64UInt64
tux64_boot_rng_random_uint64(void) {
   Tux64UInt32 hi;
   Tux64UInt32 lo;
   Tux64UInt64 retn;

   hi = tux64_boot_rng_random_uint32();
   lo = tux64_boot_rng_random_uint32();

   retn = TUX64_LITERAL_UINT64(0u);
   retn |= ((Tux64UInt64)hi << TUX64_LITERAL_UINT8(32u));
   retn |= ((Tux64UInt64)lo);

   return retn;
}

