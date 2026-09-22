/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/checksum.c - Implementations for checksum functions.         */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/checksum.h"

#include "tux64/memory.h"
#include "tux64/endian.h"

#if TUX64_IMPLEMENTATION_LIB
/*----------------------------------------------------------------------------*/

static void
tux64_checksum_fletcher_64_32_initialize(
   struct Tux64ChecksumContext * context
) {
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher;

   context_fletcher = &context->algorithm.fletcher_64_32;

   context_fletcher->sum_hi.uint       = TUX64_LITERAL_UINT32(0u);
   context_fletcher->sum_lo.uint       = TUX64_LITERAL_UINT32(0u);
   context_fletcher->word_buffer       = TUX64_LITERAL_UINT32(0u);
   context_fletcher->word_buffer_bytes = TUX64_LITERAL_UINT8(0u);
   return;
}

static void
tux64_checksum_fletcher_64_32_digest_word(
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher,
   Tux64UInt32 word
) {
   context_fletcher->sum_hi.uint += word;
   context_fletcher->sum_lo.uint += context_fletcher->sum_hi.uint;
   return;
}

static void
tux64_checksum_fletcher_64_32_word_buffer_digest(
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher
) {
   tux64_checksum_fletcher_64_32_digest_word(
      context_fletcher,
      context_fletcher->word_buffer
   );
   
   return;
}

#define TUX64_CHECKSUM_WORD_BUFFER_BYTES_MAX\
   4u

static void
tux64_checksum_fletcher_64_32_word_buffer_add(
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher,
   Tux64UInt8 byte
) {
   Tux64UInt8 position;
   Tux64UInt32 word;

   /*-------------------------------------------------------------------------*/
   /* let AA, BB, CC, DD be bytes.  we wish to shift them into the word       */
   /* buffer as follows:                                                      */
   /*                                                                         */
   /*    00000000                                                             */
   /*    AA000000                                                             */
   /*    AABB0000                                                             */
   /*    AABBCC00                                                             */
   /*    AABBCCDD                                                             */
   /*    (flush)                                                              */
   /*    00000000                                                             */
   /*      ....                                                               */
   /*                                                                         */
   /* this is nice because we don't need to worry about endianess as much.    */
   /* additionally, we implicitly zero-pad lengths which are not aligned.     */
   /* this means that when we finalize the checksum, we get zero padding for  */
   /* free.                                                                   */
   /*-------------------------------------------------------------------------*/

   if (context_fletcher->word_buffer_bytes == TUX64_LITERAL_UINT32(TUX64_CHECKSUM_WORD_BUFFER_BYTES_MAX)) {
      tux64_checksum_fletcher_64_32_word_buffer_digest(context_fletcher);
      context_fletcher->word_buffer       = TUX64_LITERAL_UINT32(0u);
      context_fletcher->word_buffer_bytes = TUX64_LITERAL_UINT32(0u);
   }
   
   position =
      TUX64_LITERAL_UINT32(TUX64_CHECKSUM_WORD_BUFFER_BYTES_MAX - 1u)
      - context_fletcher->word_buffer_bytes;

   word = (Tux64UInt32)byte << (position * TUX64_LITERAL_UINT32(8u));

   context_fletcher->word_buffer |= word;
   context_fletcher->word_buffer_bytes++;
   return;
}

static void
tux64_checksum_fletcher_64_32_digest(
   struct Tux64ChecksumContext * context,
   const Tux64UInt8 * data,
   Tux64UInt32 bytes
) {
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher;

   context_fletcher = &context->algorithm.fletcher_64_32;

   /* we could prooooobablyyyyy optimize this to align to a 4-byte bounary */
   /* and do aligned loads or whatever, but nah, don't feel like it. */
   while (bytes != TUX64_LITERAL_UINT32(0u)) {
      tux64_checksum_fletcher_64_32_word_buffer_add(context_fletcher, *data);
      data++;
      bytes--;
   }

   return;
}

static Tux64UInt8 *
tux64_checksum_fletcher_64_32_finalize(
   struct Tux64ChecksumContext * context
) {
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher;

   context_fletcher = &context->algorithm.fletcher_64_32;

   tux64_checksum_fletcher_64_32_word_buffer_digest(context_fletcher);

   context_fletcher->sum_hi.uint = context_fletcher->sum_lo.uint - context_fletcher->sum_hi.uint;
   context_fletcher->sum_hi.uint = tux64_endian_convert_uint32(context_fletcher->sum_hi.uint, TUX64_ENDIAN_FORMAT_BIG);

   return context_fletcher->sum_hi.bytes;
}

const struct Tux64ChecksumFunction
tux64_checksum_fletcher_64_32 = {
   .initialize    = tux64_checksum_fletcher_64_32_initialize,
   .digest        = tux64_checksum_fletcher_64_32_digest,
   .finalize      = tux64_checksum_fletcher_64_32_finalize,
   .digest_bytes  = TUX64_LITERAL_UINT8(TUX64_CHECKSUM_FLETCHER_64_32_DIGEST_BYTES)
};

/*----------------------------------------------------------------------------*/
#endif /* TUX64_IMPLEMENTATION_LIB */

