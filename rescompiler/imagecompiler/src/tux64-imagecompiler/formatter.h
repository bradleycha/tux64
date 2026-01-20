/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* rescompiler/imagecompiler/src/tux64-imagecompiler/formatter.h - Header for */
/*    C source code formatter.                                                */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_IMAGECOMPILER_FORMATTER_H
#define _TUX64_IMAGECOMPILER_FORMATTER_H
/*----------------------------------------------------------------------------*/

#include "tux64-imagecompiler/tux64-imagecompiler.h"
#include "tux64-imagecompiler/encoder.h"
#include "tux64-imagecompiler/compressor.h"

enum Tux64ImageCompilerFormatterGenerateStatus {
   TUX64_IMAGECOMPILER_FORMATTER_GENERATE_STATUS_OK,
   TUX64_IMAGECOMPILER_FORMATTER_GENERATE_STATUS_OUT_OF_MEMORY
};

struct Tux64ImageCompilerFormatterGeneratePayloadOk {
   Tux64UInt8 * data;
   Tux64UInt32 bytes;
};

union Tux64ImageCompilerFormatterGeneratePayload {
   struct Tux64ImageCompilerFormatterGeneratePayloadOk ok;
};

struct Tux64ImageCompilerFormatterGenerateResult {
   enum Tux64ImageCompilerFormatterGenerateStatus status;
   union Tux64ImageCompilerFormatterGeneratePayload payload;
};

/*----------------------------------------------------------------------------*/
/* Generates C source code for the encoded and compressed image data.  If the */
/* 'ok' variant is returned, then it must be manually freed with free().      */
/*----------------------------------------------------------------------------*/
struct Tux64ImageCompilerFormatterGenerateResult
tux64_imagecompiler_formatter_generate(
   const struct Tux64ImageCompilerCompressorImageData * image,
   const Tux64UInt16 color_table [TUX64_IMAGECOMPILER_ENCODER_COLORS],
   const struct Tux64String * identifier_pixels,
   const struct Tux64String * identifier_color_table
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_IMAGECOMPILER_FORMATTER_H */

