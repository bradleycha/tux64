/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/memory.c - Implementations for memory functions.             */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/memory.h"

#if TUX64_HAVE_STRING_H
/*----------------------------------------------------------------------------*/

#include <string.h>

/* we have to do this because it could be the case that our target platform */
/* has a size_t which is smaller than 32-bits. */
#if SIZE_MAX < TUX64_UINT32_MAX_LITERAL
#define TUX64_MEMORY_LIBC_SIZE_T_IS_SMALL 1
#else /* SIZE_MAX < TUX64_UINT32_MAX_LITERAL */
#define TUX64_MEMORY_LIBC_SIZE_T_IS_SMALL 0
#endif /* SIZE_MAX < TUX64_UINT32_MAX_LITERAL */

#define TUX64_MEMORY_SIZE_T_MAX TUX64_LITERAL_UINT32(SIZE_MAX)

/*----------------------------------------------------------------------------*/
#endif /* TUX64_HAVE_STRING_H */

static void
tux64_memory_copy_fallback(
   void * restrict dest,
   const void * restrict src,
   Tux64UInt32 bytes
) {
   Tux64UInt8 * restrict iter_dest;
   const Tux64UInt8 * restrict iter_src;

   iter_dest = dest;
   iter_src = src;
   while (bytes != TUX64_LITERAL_UINT32(0u)) {
      *iter_dest = *iter_src;

      iter_dest++;
      iter_src++;
      bytes--;
   }

   return;
}

#if TUX64_HAVE_STRING_H
/*----------------------------------------------------------------------------*/

static void
tux64_memory_copy_libc(
   void * restrict dest,
   const void * restrict src,
   Tux64UInt32 bytes
) {
   Tux64UInt8 * restrict iter_dest;
   const Tux64UInt8 * restrict iter_src;

   iter_dest = dest;
   iter_src = src;

#if TUX64_MEMORY_LIBC_SIZE_T_IS_SMALL
   while (bytes > TUX64_MEMORY_SIZE_T_MAX) {
      (void)memcpy(iter_dest, iter_src, TUX64_MEMORY_SIZE_T_MAX);
      iter_dest += TUX64_MEMORY_SIZE_T_MAX;
      iter_src += TUX64_MEMORY_SIZE_T_MAX;
      bytes -= TUX64_MEMORY_SIZE_T_MAX;
   }
#endif /* TUX64_MEMORY_LIBC_SIZE_T_IS_SMALL */

   (void)memcpy(iter_dest, iter_src, bytes);

   return;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_HAVE_STRING_H */

void
tux64_memory_copy(
   void * restrict dest,
   const void * restrict src,
   Tux64UInt32 bytes
) {
#if TUX64_HAVE_STRING_H
   tux64_memory_copy_libc(dest, src, bytes);
   (void)tux64_memory_copy_fallback;
#else /* TUX64_HAVE_STRING_H */
   tux64_memory_copy_fallback(dest, src, bytes);
#endif /* TUX64_HAVE_STRING_H */

   return;
}

static void
tux64_memory_fill_fallback(
   void * restrict dest,
   const void * restrict sentinel,
   Tux64UInt32 elements,
   Tux64UInt32 bytes_per_element
) {
   Tux64UInt8 * restrict iter_dest;

   iter_dest = dest;
   while (elements != TUX64_LITERAL_UINT32(0u)) {
      tux64_memory_copy(iter_dest, sentinel, bytes_per_element);
      iter_dest += bytes_per_element;
      elements--;
   }

   return;
}

#if TUX64_HAVE_STRING_H
/*----------------------------------------------------------------------------*/

static void
tux64_memory_fill_libc(
   void * restrict dest,
   const void * restrict sentinel,
   Tux64UInt32 elements,
   Tux64UInt32 bytes_per_element
) {
   Tux64UInt8 * restrict iter_dest;
   Tux64UInt8 byte;

   if (bytes_per_element != TUX64_LITERAL_UINT32(1u)) {
      tux64_memory_fill_fallback(dest, sentinel, elements, bytes_per_element);
      return;
   }

   iter_dest = dest;
   byte = *(const Tux64UInt8 * restrict)sentinel;

#if TUX64_MEMORY_LIBC_SIZE_T_IS_SMALL
   while (elements > TUX64_MEMORY_SIZE_T_MAX) {
      (void)memset(iter_dest, byte, TUX64_MEMORY_SIZE_T_MAX);
      iter_dest += TUX64_MEMORY_SIZE_T_MAX;
      elements -= TUX64_MEMORY_SIZE_T_MAX;
   }
#endif /* TUX64_MEMORY_LIBC_SIZE_T_IS_SMALL */

   (void)memset(iter_dest, byte, elements);

   return;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_HAVE_STRING_H */

void
tux64_memory_fill(
   void * restrict dest,
   const void * restrict sentinel,
   Tux64UInt32 elements,
   Tux64UInt32 bytes_per_element
) {
#if TUX64_HAVE_STRING_H
   tux64_memory_fill_libc(dest, sentinel, elements, bytes_per_element);
#else
   tux64_memory_fill_fallback(dest, sentinel, elements, bytes_per_element);
#endif

   return;
}

Tux64Boolean
tux64_memory_compare(
   const void * restrict data_lhs,
   const void * restrict data_rhs,
   Tux64UInt32 bytes_lhs,
   Tux64UInt32 bytes_rhs
) {
   if (bytes_lhs != bytes_rhs) {
      return TUX64_BOOLEAN_FALSE;
   }

   return tux64_memory_compare_with_equal_lengths(data_lhs, data_rhs, bytes_lhs);
}

static Tux64Boolean
tux64_memory_compare_with_equal_lengths_fallback(
   const void * restrict data_lhs,
   const void * restrict data_rhs,
   Tux64UInt32 bytes
) {
   const Tux64UInt8 * restrict iter_data_lhs;
   const Tux64UInt8 * restrict iter_data_rhs;

   iter_data_lhs = data_lhs;
   iter_data_rhs = data_rhs;

   while (bytes != TUX64_LITERAL_UINT32(0u)) {
      if (*iter_data_lhs != *iter_data_rhs) {
         return TUX64_BOOLEAN_FALSE;
      }

      iter_data_lhs++;
      iter_data_rhs++;
      bytes--;
   }

   return TUX64_BOOLEAN_TRUE;
}

#if TUX64_HAVE_STRING_H
/*----------------------------------------------------------------------------*/

static Tux64Boolean
tux64_memory_compare_with_equal_lengths_libc(
   const void * restrict data_lhs,
   const void * restrict data_rhs,
   Tux64UInt32 bytes
) {
   const Tux64UInt8 * restrict iter_data_lhs;
   const Tux64UInt8 * restrict iter_data_rhs;

   iter_data_lhs = data_lhs;
   iter_data_rhs = data_rhs;

#if TUX64_MEMORY_LIBC_SIZE_T_IS_SMALL
   while (bytes > TUX64_MEMORY_SIZE_T_MAX) {
      if (memcmp(iter_data_lhs, iter_data_rhs, TUX64_MEMORY_SIZE_T_MAX) != 0) {
         return TUX64_BOOLEAN_FALSE;
      }

      iter_data_lhs += TUX64_MEMORY_SIZE_T_MAX;
      iter_data_rhs += TUX64_MEMORY_SIZE_T_MAX;
      bytes -= TUX64_MEMORY_SIZE_T_MAX;
   }
#endif /* TUX64_MEMORY_LIBC_SIZE_T_IS_SMALL */

   if (memcmp(iter_data_lhs, iter_data_rhs, bytes) != 0) {
      return TUX64_BOOLEAN_FALSE;
   }

   return TUX64_BOOLEAN_TRUE;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_HAVE_STRING_H */

Tux64Boolean
tux64_memory_compare_with_equal_lengths(
   const void * restrict data_lhs,
   const void * restrict data_rhs,
   Tux64UInt32 bytes
) {
   Tux64Boolean retn;

#if TUX64_HAVE_STRING_H
   retn = tux64_memory_compare_with_equal_lengths_libc(data_lhs, data_rhs, bytes);
   (void)tux64_memory_compare_with_equal_lengths_fallback;
#else /* TUX64_HAVE_STRING_H */
   retn = tux64_memory_compare_with_equal_lengths_fallback(data_lhs, data_rhs, bytes);
#endif /* TUX64_HAVE_STRING_H */

   return retn;
}

struct Tux64MemoryFindResult
tux64_memory_find(
   const void * restrict data,
   const void * restrict sentinel,
   Tux64UInt32 elements,
   Tux64UInt32 bytes_per_element
) {
   struct Tux64MemoryFindResult result;
   const Tux64UInt8 * restrict iter_data;

   iter_data = data;
   result.payload.found.position = TUX64_LITERAL_UINT32(0u);

   while (elements != TUX64_LITERAL_UINT32(0u)) {
      if (tux64_memory_compare_with_equal_lengths(
         iter_data,
         sentinel,
         bytes_per_element
      ) == TUX64_BOOLEAN_TRUE) {
         result.status = TUX64_MEMORY_FIND_STATUS_FOUND;
         return result;
      }

      iter_data += bytes_per_element;
      result.payload.found.position++;
      elements--;
   }

   result.status = TUX64_MEMORY_FIND_STATUS_MISSING;
   return result;
}

