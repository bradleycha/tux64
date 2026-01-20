/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/imagecompiler/src/tux64-imagecompiler/encoder.c -              */
/*    Implementations for CI4/RGBA5551 pixel encoder.                         */
/*----------------------------------------------------------------------------*/

#include "tux64-imagecompiler/tux64-imagecompiler.h"
#include "tux64-imagecompiler/encoder.h"

#define TUX64_IMAGECOMPILER_ENCODER_RGBA8888_BYTES_PER_PIXEL \
   (4u)
#define TUX64_IMAGECOMPILER_ENCODER_RGBA8888_BYTES_TOTAL \
   ( \
      TUX64_IMAGECOMPILER_ENCODER_RGBA8888_BYTES_PER_PIXEL * \
      TUX64_IMAGECOMPILER_ENCODER_PIXELS \
   )

#define TUX64_IMAGECOMPILER_ENCODER_COLOR_TABLE_EMPTY_COLOR \
   (0x0000u)
#define TUX64_IMAGECOMPILER_ENCODER_COLOR_TABLE_IDX_INVALID \
   (0xffu)

static void
tux64_imagecompiler_encoder_color_table_initialize(
   Tux64UInt16 color_table [TUX64_IMAGECOMPILER_ENCODER_COLORS]
) {
   Tux64UInt8 i;

   i = TUX64_LITERAL_UINT8(TUX64_IMAGECOMPILER_ENCODER_COLORS);
   do {
      color_table[i - 1u] = TUX64_LITERAL_UINT16(TUX64_IMAGECOMPILER_ENCODER_COLOR_TABLE_EMPTY_COLOR);
      i--;
   } while (i != TUX64_LITERAL_UINT8(1u));

   return;
}

static Tux64UInt8
tux64_imagecompiler_encoder_color_table_load_pixel(
   Tux64UInt16 color_table [TUX64_IMAGECOMPILER_ENCODER_COLORS],
   Tux64UInt16 pixel
) {
   Tux64UInt8 i;
   Tux64UInt16 color;

   /* assign transparent pixels to color index zero. */
   if ((pixel & TUX64_LITERAL_UINT16(0x0001u)) == TUX64_LITERAL_UINT16(0x0000u)) {
      return TUX64_LITERAL_UINT8(0u);
   }

   /* search for either the index of a loaded color, or load a new color */
   i = TUX64_LITERAL_UINT8(TUX64_IMAGECOMPILER_ENCODER_COLORS);
   do {
      color = color_table[i - 1u];

      if (color == TUX64_LITERAL_UINT16(TUX64_IMAGECOMPILER_ENCODER_COLOR_TABLE_EMPTY_COLOR)) {
         color_table[i - 1u] = pixel;
         return i;
      }
      if (color == pixel) {
         return i;
      }

      i--;
   } while (i != TUX64_LITERAL_UINT8(0u));

   /* no free space and not found? too many colors! */
   return TUX64_LITERAL_UINT8(TUX64_IMAGECOMPILER_ENCODER_COLOR_TABLE_IDX_INVALID);
}

static Tux64UInt16
tux64_imagecompiler_encoder_rgba8888_to_rgba5551(
   Tux64UInt8 r,
   Tux64UInt8 g,
   Tux64UInt8 b,
   Tux64UInt8 a
) {
   Tux64UInt16 pixel;

   /* truncate alpha to 1-bit if it's less than 50% opacity */
   if (a < TUX64_LITERAL_UINT8(0x80u)) {
      return TUX64_LITERAL_UINT16(0x0000u);
   }

   pixel = TUX64_LITERAL_UINT16(0x0001u);
   pixel |= ((r / TUX64_LITERAL_UINT8(8u)) << TUX64_LITERAL_UINT8(11u));
   pixel |= ((g / TUX64_LITERAL_UINT8(8u)) << TUX64_LITERAL_UINT8( 6u));
   pixel |= ((b / TUX64_LITERAL_UINT8(8u)) << TUX64_LITERAL_UINT8( 1u));

   return pixel;
}

static void
tux64_imagecompiler_encoder_load_pixel_tuple(
   const Tux64UInt8 * ptr,
   Tux64UInt16 output [2u]
) {
   Tux64UInt8 i;
   Tux64UInt16 * iter_pixel;
   Tux64UInt8 r;
   Tux64UInt8 g;
   Tux64UInt8 b;
   Tux64UInt8 a;

   i = TUX64_LITERAL_UINT8(2u);
   iter_pixel = output;
   do {
      r = *ptr++;
      g = *ptr++;
      b = *ptr++;
      a = *ptr++;

      *iter_pixel++ = tux64_imagecompiler_encoder_rgba8888_to_rgba5551(r, g, b, a);
      i--;
   } while (i != TUX64_LITERAL_UINT8(0u));

   return;
}

static enum Tux64ImageCompilerEncoderStatus
tux64_imagecompiler_encoder_encode_pixel_tuple(
   Tux64UInt16 pixel_tuple [2u],
   Tux64UInt8 * output_pixel_tuples,
   Tux64UInt16 color_table [TUX64_IMAGECOMPILER_ENCODER_COLORS]
) {
   Tux64UInt8 idx_pixel [2u];
   Tux64UInt8 i;
   Tux64UInt8 out;

   i = TUX64_LITERAL_UINT8(TUX64_ARRAY_ELEMENTS(idx_pixel));
   do {
      idx_pixel[i - 1u] = tux64_imagecompiler_encoder_color_table_load_pixel(color_table, pixel_tuple[i - 1u]);
      if (idx_pixel[i - 1u] == TUX64_IMAGECOMPILER_ENCODER_COLOR_TABLE_IDX_INVALID) {
         return TUX64_IMAGECOMPILER_ENCODER_STATUS_TOO_MANY_COLORS;
      }

      i--;
   } while (i != TUX64_LITERAL_UINT8(0u));

   out = TUX64_LITERAL_UINT8(0u);
   out |= (idx_pixel[0u] << TUX64_LITERAL_UINT8(4u));
   out |= (idx_pixel[1u]);
   *output_pixel_tuples = out;
   
   return TUX64_IMAGECOMPILER_ENCODER_STATUS_OK;
}

enum Tux64ImageCompilerEncoderStatus
tux64_imagecompiler_encoder_encode(
   const Tux64UInt8 * input,
   Tux64UInt32 input_bytes,
   struct Tux64ImageCompilerEncoderImageData * output
) {
   Tux64UInt16 pixel_tuple [2u];
   const Tux64UInt8 * iter_input;
   Tux64UInt8 * iter_output_pixel_tuples;
   Tux64UInt32 pixels_remaining;
   enum Tux64ImageCompilerEncoderStatus status;

   if (input_bytes != TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ENCODER_RGBA8888_BYTES_TOTAL)) {
      return TUX64_IMAGECOMPILER_ENCODER_STATUS_INVALID_IMAGE_FORMAT;
   }

   tux64_imagecompiler_encoder_color_table_initialize(output->color_table);

   iter_input = input;
   iter_output_pixel_tuples = output->pixel_tuples;

   pixels_remaining = TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ENCODER_PIXELS);
   do {
      tux64_imagecompiler_encoder_load_pixel_tuple(iter_input, pixel_tuple);

      status = tux64_imagecompiler_encoder_encode_pixel_tuple(
         pixel_tuple,
         iter_output_pixel_tuples,
         output->color_table
      );
      if (status != TUX64_IMAGECOMPILER_ENCODER_STATUS_OK) {
         return status;
      }

      pixels_remaining -= TUX64_LITERAL_UINT32(TUX64_ARRAY_ELEMENTS(pixel_tuple));
      iter_input += TUX64_LITERAL_UINT32(TUX64_IMAGECOMPILER_ENCODER_RGBA8888_BYTES_PER_PIXEL * 2u);
      iter_output_pixel_tuples += TUX64_LITERAL_UINT32(1u);
   } while (pixels_remaining != TUX64_LITERAL_UINT32(0u));

   return TUX64_IMAGECOMPILER_ENCODER_STATUS_OK;
}

