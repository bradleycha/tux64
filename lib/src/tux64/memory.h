/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/memory.h - Header for memory functions.                      */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_MEMORY_H
#define _TUX64_MEMORY_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

/*----------------------------------------------------------------------------*/
/* Copy 'bytes' number of bytes from 'src' to 'dest'.                         */
/*----------------------------------------------------------------------------*/
void
tux64_memory_copy(
   void * restrict dest,
   const void * restrict src,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Fill 'dest' with 'elements' copies of 'sentinel', where 'sentinel' is      */
/* 'bytes_per_element' number of bytes.                                       */
/*----------------------------------------------------------------------------*/
void
tux64_memory_fill(
   void * restrict dest,
   const void * restrict sentinel,
   Tux64UInt32 elements,
   Tux64UInt32 bytes_per_element
);

/*----------------------------------------------------------------------------*/
/* Compares 'data_lhs' and 'data_rhs' for equality.  If the lengths do not    */
/* match or the contents to not match, this evaluates to false.               */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_memory_compare(
   const void * restrict data_lhs,
   const void * restrict data_rhs,
   Tux64UInt32 bytes_lhs,
   Tux64UInt32 bytes_rhs
);

/*----------------------------------------------------------------------------*/
/* Same as tux64_memory_compare(), but assume 'data_lhs' is the same length   */
/* as 'data_rhs'.                                                             */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_memory_compare_with_equal_lengths(
   const void * restrict data_lhs,
   const void * restrict data_rhs,
   Tux64UInt32 bytes
);

/*----------------------------------------------------------------------------*/
/* Checks if a block of data starts with another block of data.               */
/*----------------------------------------------------------------------------*/
Tux64Boolean
tux64_memory_compare_start(
   const void * restrict data,
   const void * restrict prefix,
   Tux64UInt32 bytes_data,
   Tux64UInt32 bytes_prefix 
);

enum Tux64MemoryFindStatus {
   TUX64_MEMORY_FIND_STATUS_FOUND,
   TUX64_MEMORY_FIND_STATUS_MISSING
};

struct Tux64MemoryFindResultPayloadFound {
   Tux64UInt32 position;
};

union Tux64MemoryFindResultPayload {
   struct Tux64MemoryFindResultPayloadFound found;
};

struct Tux64MemoryFindResult {
   enum Tux64MemoryFindStatus status;
   union Tux64MemoryFindResultPayload payload;
};

/*----------------------------------------------------------------------------*/
/* Attempt to find 'sentinel' in 'data', given 'sentinel' is                  */
/* 'bytes_per_element' number of bytes and there are 'elements' number of     */
/* sentinels in 'data'.  The result struct will return the position in 'data' */
/* where 'sentinel' was found as an offset from the start of 'data', where    */
/* each offset unit represents 'bytes_per_element' number of bytes.           */
/*----------------------------------------------------------------------------*/
struct Tux64MemoryFindResult
tux64_memory_find(
   const void * restrict data,
   const void * restrict sentinel,
   Tux64UInt32 elements,
   Tux64UInt32 bytes_per_element
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_MEMORY_H */

