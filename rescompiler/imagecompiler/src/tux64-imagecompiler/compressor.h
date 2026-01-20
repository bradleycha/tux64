/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/imagecompiler/src/tux64-imagecompiler/compressor.h - Header    */
/*    for RLE compressor.                                                     */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_IMAGECOMPILER_COMPRESSOR_H
#define _TUX64_IMAGECOMPILER_COMPRESSOR_H
/*----------------------------------------------------------------------------*/

#include "tux64-imagecompiler/tux64-imagecompiler.h"
#include "tux64-imagecompiler/encoder.h"

#define TUX64_IMAGECOMPILER_COMPRESSOR_MAX_BYTES \
   ( \
      TUX64_IMAGECOMPILER_ENCODER_PIXEL_TUPLES + \
      ( \
        TUX64_IMAGECOMPILER_ENCODER_PIXEL_TUPLES / \
        128u \
      ) \
   )

struct Tux64ImageCompilerCompressorImageData {
   Tux64UInt8 bytes [TUX64_IMAGECOMPILER_COMPRESSOR_MAX_BYTES];
   Tux64UInt32 length;
};

/*----------------------------------------------------------------------------*/
/* Compresses encoded image data with the RLE compression algorithm in        */
/* tux64-boot.                                                                */
/*----------------------------------------------------------------------------*/
struct Tux64ImageCompilerCompressorImageData
tux64_imagecompiler_compressor_compress(
   const struct Tux64ImageCompilerEncoderImageData * image
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_IMAGECOMPILER_COMPRESSOR_H */

