/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/checksum.h - Header for checksum functions.                  */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_CHECKSUM_H
#define _TUX64_CHECKSUM_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

struct Tux64ChecksumContext;

/*----------------------------------------------------------------------------*/
/* Initializes the checksum context for the given function.                   */
/*----------------------------------------------------------------------------*/
typedef void (*Tux64ChecksumFunctionInitialize)(
   struct Tux64ChecksumContext * context
);

/*----------------------------------------------------------------------------*/
/* Digests byte data into the checksum function.  'context' should have been  */
/* first initialized before digesting.                                        */
/*----------------------------------------------------------------------------*/
typedef void (*Tux64ChecksumFunctionDigest)(
   struct Tux64ChecksumContext * context,
   const Tux64UInt8 * data,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Finalizes the checksum context, returning a pointer to the final digest    */
/* value.  The returned digest value pointer is valid until 'context' is      */
/* re-initialized.                                                            */
/*----------------------------------------------------------------------------*/
typedef Tux64UInt8 * (*Tux64ChecksumFunctionFinalize)(
   struct Tux64ChecksumContext * context
);

struct Tux64ChecksumFunction {
   Tux64ChecksumFunctionInitialize initialize;
   Tux64ChecksumFunctionDigest digest;
   Tux64ChecksumFunctionFinalize finalize;
   Tux64UInt8 digest_bytes;
};

/*----------------------------------------------------------------------------*/
/* A variation of the Fletcher64 algorithm.  Words are treated as big-endian, */
/* and the returned digest value is a 32-bit word that is the result of       */
/* performing wrapping subtraction of the second sum minus the first sum.     */
/* The modulus used is 2^32.                                                  */
/*----------------------------------------------------------------------------*/
#define TUX64_CHECKSUM_FLETCHER_64_32_DIGEST_BYTES\
   4u
extern const struct Tux64ChecksumFunction
tux64_checksum_fletcher_64_32;

struct _Tux64ChecksumContextAlgorithmFletcher6432 {
   Tux64UInt32 sum_hi;
   Tux64UInt32 sum_lo;
   Tux64UInt8 word_buffer [sizeof(Tux64UInt32)];
   Tux64UInt8 word_buffer_capacity;
};

union _Tux64ChecksumContextAlgorithm {
   struct _Tux64ChecksumContextAlgorithmFletcher6432 fletcher_64_32;
};

struct Tux64ChecksumContext {
   union _Tux64ChecksumContextAlgorithm algorithm;
};

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_CHECKSUM_H */

