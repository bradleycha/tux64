/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-mkrom/builder.c - Implementations for ROM builder.         */
/*----------------------------------------------------------------------------*/

#include "tux64-mkrom/tux64-mkrom.h"
#include "tux64-mkrom/builder.h"

struct Tux64MkromBuilderMeasureResult
tux64_mkrom_builder_measure_and_verify(
   const struct Tux64MkromBuilderInput * input
) {
   struct Tux64MkromBuilderMeasureResult result;

   /* TODO: implement */
   (void)input;
   result.status = TUX64_MKROM_BUILDER_MEASURE_STATUS_BAD_LENGTH_BOOTLOADER_STAGE0;
   result.payload.bad_length.bytes_max = TUX64_LITERAL_UINT32(0u);
   result.payload.bad_length.bytes_given_unaligned = TUX64_LITERAL_UINT32(0u);
   result.payload.bad_length.bytes_given_aligned = TUX64_LITERAL_UINT32(0u);
   return result;
}

void
tux64_mkrom_builder_construct(
   const struct Tux64MkromBuilderInput * input,
   Tux64UInt8 * output
) {
   /* TODO: implement */
   (void)input;
   (void)output;
   return;
}

