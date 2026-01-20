/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/imagecompiler/src/tux64-imagecompiler/compressor.c -           */
/*    Implementations for RLE compressor.                                     */
/*----------------------------------------------------------------------------*/

#include "tux64-imagecompiler/tux64-imagecompiler.h"
#include "tux64-imagecompiler/compressor.h"

#include <tux64/memory.h>

#define TUX64_IMAGECOMPILER_COMPRESSOR_MAX_RUN_LENGTH \
   (0x80u)

static Tux64UInt32
tux64_imagecompiler_compressor_next_run_length(
   const Tux64UInt8 * pixel_tuples,
   Tux64UInt32 remaining
) {
   Tux64UInt32 length;
   Tux64UInt8 last_tuple;
   Tux64UInt8 current_tuple;

   last_tuple = *pixel_tuples;
   pixel_tuples++;
   remaining--;

   length = TUX64_LITERAL_UINT32(1u);

   while (remaining != TUX64_LITERAL_UINT32(0u)) {
      current_tuple = *pixel_tuples;

      if (current_tuple != last_tuple) {
         break;
      }
      if (length == TUX64_LITERAL_UINT8(TUX64_IMAGECOMPILER_COMPRESSOR_MAX_RUN_LENGTH)) {
         break;
      }

      last_tuple = current_tuple;

      pixel_tuples++;
      remaining--;
      length++;
   }

   return length;
}

#define TUX64_IMAGECOMPILER_COMPRESSOR_RLE_COMMAND_STRAIGHT \
   (0x00u)
#define TUX64_IMAGECOMPILER_COMPRESSOR_RLE_COMMAND_COMPRESSED \
   (0x80u)

static Tux64UInt32
tux64_imagecompiler_compressor_format_run_straight(
   Tux64UInt8 * output,
   Tux64UInt8 run_length,
   const Tux64UInt8 * pixel_tuples
) {
   Tux64UInt32 bytes;

   bytes = TUX64_LITERAL_UINT32(0u);

   *output++ = TUX64_LITERAL_UINT8(TUX64_IMAGECOMPILER_COMPRESSOR_RLE_COMMAND_STRAIGHT) | (run_length - TUX64_LITERAL_UINT8(1u));
   bytes++;

   tux64_memory_copy(
      output,
      pixel_tuples,
      (Tux64UInt32)run_length
   );
   output += run_length;
   bytes += run_length;

   return bytes;
}

static Tux64UInt32
tux64_imagecompiler_compressor_format_run_compressed(
   Tux64UInt8 * output,
   Tux64UInt8 run_length,
   Tux64UInt8 pixel_tuple
) {
   Tux64UInt32 bytes;

   bytes = TUX64_LITERAL_UINT32(0u);

   *output++ = TUX64_LITERAL_UINT8(TUX64_IMAGECOMPILER_COMPRESSOR_RLE_COMMAND_COMPRESSED) | (run_length - TUX64_LITERAL_UINT8(1u));
   bytes++;

   *output++ = pixel_tuple;
   bytes++;

   return bytes;
}

struct Tux64ImageCompilerCompressorImageData
tux64_imagecompiler_compressor_compress(
   const struct Tux64ImageCompilerEncoderImageData * image
) {
   struct Tux64ImageCompilerCompressorImageData compressed;
   const Tux64UInt8 * iter_image;
   const Tux64UInt8 * iter_image_run;
   Tux64UInt8 * iter_compressed;
   Tux64UInt32 remaining_pixel_tuples;
   Tux64UInt32 run_length;
   Tux64UInt32 compression_length;
   Tux64Boolean straight_run;
   Tux64UInt8 straight_run_length;

   iter_image = image->pixel_tuples;
   iter_image_run = iter_image;
   iter_compressed = compressed.bytes;

   straight_run = TUX64_BOOLEAN_FALSE;
   straight_run_length = TUX64_LITERAL_UINT8(0u);

   remaining_pixel_tuples = TUX64_LITERAL_UINT32(TUX64_ARRAY_ELEMENTS(image->pixel_tuples));
   compressed.length = TUX64_LITERAL_UINT32(0u);

   do {
      run_length = tux64_imagecompiler_compressor_next_run_length(
         iter_image,
         remaining_pixel_tuples
      );

      /* we can get optimal compression by only RLE encoding runs of length */
      /* greater than two.  a run of two has equal encoding size as an */
      /* uncompressed run of two, and a run of three is less.  also break the */
      /* run if we reach the max length */
      if (
         run_length == TUX64_LITERAL_UINT32(1u) &&
         straight_run_length != TUX64_LITERAL_UINT8(TUX64_IMAGECOMPILER_COMPRESSOR_MAX_RUN_LENGTH)
      ) {
         straight_run = TUX64_BOOLEAN_TRUE;
         straight_run_length++;
         iter_image++;
         remaining_pixel_tuples--;
         continue;
      }

      /* if the last run was uncompressed, encode it first. */
      if (straight_run == TUX64_BOOLEAN_TRUE) {
         compression_length = tux64_imagecompiler_compressor_format_run_straight(
            iter_compressed,
            straight_run_length,
            iter_image_run
         );

         iter_compressed += compression_length;
         compressed.length += compression_length;

         straight_run = TUX64_BOOLEAN_FALSE;
         straight_run_length = TUX64_LITERAL_UINT8(0u);
      }

      /* if we now have RLE data to encode, do that. */
      if (run_length != TUX64_LITERAL_UINT32(1u)) {
         compression_length = tux64_imagecompiler_compressor_format_run_compressed(
            iter_compressed,
            run_length,
            *iter_image
         );

         iter_compressed += compression_length;
         compressed.length += compression_length;

         iter_image += run_length;
         remaining_pixel_tuples -= run_length;
      }

      iter_image_run = iter_image;
   } while (remaining_pixel_tuples != TUX64_LITERAL_UINT32(0u));

   /* make sure to format any remainder runs if the end of the image is */
   /* uncompressed. */
   if (straight_run == TUX64_BOOLEAN_TRUE) {
      compression_length = tux64_imagecompiler_compressor_format_run_straight(
         iter_compressed,
         straight_run_length,
         iter_image_run
      );
      compressed.length += compression_length;
   }

   return compressed;
}

