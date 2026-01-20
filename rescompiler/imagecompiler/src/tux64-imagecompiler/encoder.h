/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/imagecompiler/src/tux64-imagecompiler/encoder.h - Header for   */
/*    CI4/RGBA5551 pixel encoder.                                             */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_IMAGECOMPILER_ENCODER_H
#define _TUX64_IMAGECOMPILER_ENCODER_H
/*----------------------------------------------------------------------------*/

#include "tux64-imagecompiler/tux64-imagecompiler.h"

#define TUX64_IMAGECOMPILER_ENCODER_IMAGE_WIDTH \
   64u
#define TUX64_IMAGECOMPILER_ENCODER_IMAGE_HEIGHT \
   64u
#define TUX64_IMAGECOMPILER_ENCODER_BITS_PER_PIXEL \
   4u
#define TUX64_IMAGECOMPILER_ENCODER_BITS_PER_COLOR \
   16u
#define TUX64_IMAGECOMPILER_ENCODER_COLORS \
   ((1u << TUX64_IMAGECOMPILER_ENCODER_BITS_PER_PIXEL) - 1u)

#define TUX64_IMAGECOMPILER_ENCODER_PIXELS \
   ( \
      TUX64_IMAGECOMPILER_ENCODER_IMAGE_WIDTH * \
      TUX64_IMAGECOMPILER_ENCODER_IMAGE_HEIGHT \
   )
#define TUX64_IMAGECOMPILER_ENCODER_PIXEL_TUPLES \
   ( \
      ( \
         TUX64_IMAGECOMPILER_ENCODER_PIXELS * \
         TUX64_IMAGECOMPILER_ENCODER_BITS_PER_PIXEL \
      ) / \
      8u \
   )

struct Tux64ImageCompilerEncoderImageData {
   Tux64UInt8 pixel_tuples [TUX64_IMAGECOMPILER_ENCODER_PIXEL_TUPLES];
   Tux64UInt16 color_table [TUX64_IMAGECOMPILER_ENCODER_COLORS];
};

enum Tux64ImageCompilerEncoderStatus {
   TUX64_IMAGECOMPILER_ENCODER_STATUS_OK,
   TUX64_IMAGECOMPILER_ENCODER_STATUS_INVALID_IMAGE_FORMAT,
   TUX64_IMAGECOMPILER_ENCODER_STATUS_TOO_MANY_COLORS
};

/*----------------------------------------------------------------------------*/
/* Encodes an RGBA8888 image into the CI4/RGBA5551 format used by tux64-boot. */
/* The output image data is uncompressed, and should be RLE compressed.       */
/*----------------------------------------------------------------------------*/
enum Tux64ImageCompilerEncoderStatus
tux64_imagecompiler_encoder_encode(
   const Tux64UInt8 * input,
   Tux64UInt32 input_bytes,
   struct Tux64ImageCompilerEncoderImageData * output
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_IMAGECOMPILER_ENCODER_H */

