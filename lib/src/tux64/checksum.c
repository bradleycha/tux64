/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/checksum.c - Implementations for checksum functions.         */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/checksum.h"

#include "tux64/memory.h"
#include "tux64/endian.h"

static void
tux64_checksum_fletcher_64_32_initialize(
   struct Tux64ChecksumContext * context
) {
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher;
   Tux64UInt8 sentinel_zero;

   context_fletcher = &context->algorithm.fletcher_64_32;

   context_fletcher->sum_hi.uint = TUX64_LITERAL_UINT32(0u);
   context_fletcher->sum_lo.uint = TUX64_LITERAL_UINT32(0u);

   sentinel_zero = TUX64_LITERAL_UINT8(0x00);
   tux64_memory_fill(
      context_fletcher->word_buffer,
      &sentinel_zero,
      TUX64_LITERAL_UINT32(TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer)),
      TUX64_LITERAL_UINT32(sizeof(sentinel_zero))
   );

   context_fletcher->word_buffer_capacity = TUX64_LITERAL_UINT8(TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer));

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
tux64_checksum_fletcher_64_32_flush_word_buffer(
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher
) {
   Tux64UInt8 sentinel_zero;
   union Tux64Int32 word;

   /* don't do anything if the buffer is empty */
   if (context_fletcher->word_buffer_capacity == TUX64_LITERAL_UINT32(TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer))) {
      return;
   }

   /* pad the buffer with zeroes */
   sentinel_zero = TUX64_LITERAL_UINT8(0x00);
   tux64_memory_fill(
      &context_fletcher->word_buffer[TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer) - context_fletcher->word_buffer_capacity],
      &sentinel_zero,
      (Tux64UInt32)context_fletcher->word_buffer_capacity,
      TUX64_LITERAL_UINT32(sizeof(sentinel_zero))
   );

   /* take the word from the buffer and digest it */
   tux64_endian_convert_copy(
      word.bytes,
      context_fletcher->word_buffer,
      TUX64_LITERAL_UINT32(sizeof(context_fletcher->word_buffer)),
      TUX64_ENDIAN_FORMAT_BIG
   );
   context_fletcher->word_buffer_capacity = TUX64_LITERAL_UINT8(TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer));

   tux64_checksum_fletcher_64_32_digest_word(context_fletcher, word.uint);

   return;
}

static Tux64UInt32
tux64_checksum_fletcher_64_32_fill_word_buffer(
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher,
   const Tux64UInt8 * data,
   Tux64UInt32 bytes
) {
   Tux64UInt32 taken;
   union Tux64Int32 word;

   if (context_fletcher->word_buffer_capacity == TUX64_LITERAL_UINT32(TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer))) {
      return TUX64_LITERAL_UINT32(0u);
   }

   /* if we can't complete the word, simply copy into the word buffer */
   if (bytes < context_fletcher->word_buffer_capacity) {
      tux64_memory_copy(
         &context_fletcher->word_buffer[TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer) - context_fletcher->word_buffer_capacity],
         data,
         bytes * TUX64_LITERAL_UINT32(sizeof(Tux64UInt8))
      );
      context_fletcher->word_buffer_capacity -= (Tux64UInt8)bytes;

      return bytes;
   }

   /* fill the word buffer then digest the complete word */
   taken = (Tux64UInt32)context_fletcher->word_buffer_capacity;
   tux64_memory_copy(
      &context_fletcher->word_buffer[TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer) - context_fletcher->word_buffer_capacity],
      data,
      (Tux64UInt32)context_fletcher->word_buffer_capacity
   );
   context_fletcher->word_buffer_capacity = TUX64_LITERAL_UINT8(TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer));
   tux64_endian_convert_copy(
      word.bytes,
      context_fletcher->word_buffer,
      TUX64_LITERAL_UINT32(sizeof(word)),
      TUX64_ENDIAN_FORMAT_BIG
   );

   tux64_checksum_fletcher_64_32_digest_word(context_fletcher, word.uint);

   return taken;
}

static void
tux64_checksum_fletcher_64_32_digest(
   struct Tux64ChecksumContext * context,
   const Tux64UInt8 * data,
   Tux64UInt32 bytes
) {
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher;
   const Tux64UInt8 * iter_data;
   Tux64UInt32 bytes_taken;
   union Tux64Int32 word;

   context_fletcher = &context->algorithm.fletcher_64_32;
   iter_data = data;

   /* attempt to complete the word buffer first */
   bytes_taken = tux64_checksum_fletcher_64_32_fill_word_buffer(context_fletcher, iter_data, bytes);
   iter_data += bytes_taken;
   bytes -= bytes_taken;

   /* digest all whole words */
   while (bytes >= TUX64_LITERAL_UINT32(sizeof(Tux64UInt32))) {
      tux64_endian_convert_copy(
         word.bytes,
         iter_data,
         TUX64_LITERAL_UINT32(sizeof(Tux64UInt32)),
         TUX64_ENDIAN_FORMAT_BIG
      );

      tux64_checksum_fletcher_64_32_digest_word(context_fletcher, word.uint);

      iter_data += sizeof(Tux64UInt32);
      bytes -= TUX64_LITERAL_UINT32(sizeof(Tux64UInt32));
   }

   /* copy any remaining bytes into the word buffer */
   tux64_memory_copy(
      context_fletcher->word_buffer,
      iter_data,
      bytes
   );
   context_fletcher->word_buffer_capacity = TUX64_LITERAL_UINT8(TUX64_ARRAY_ELEMENTS(context_fletcher->word_buffer)) - (Tux64UInt8)bytes;

   return;
}

static Tux64UInt8 *
tux64_checksum_fletcher_64_32_finalize(
   struct Tux64ChecksumContext * context
) {
   struct _Tux64ChecksumContextAlgorithmFletcher6432 * context_fletcher;

   context_fletcher = &context->algorithm.fletcher_64_32;

   tux64_checksum_fletcher_64_32_flush_word_buffer(context_fletcher);

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

