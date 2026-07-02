/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-sramdumper/format.h - Header for SRAM dump formatter.      */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_SRAMDUMPER_FORMAT_H
#define _TUX64_SRAMDUMPER_FORMAT_H
/*----------------------------------------------------------------------------*/

#include "tux64-sramdumper/tux64-sramdumper.h"
#include "tux64-sramdumper/parse.h"

/* we could compute this properly, but fuck it.  good enough.  i just want to */
/* get this shit to work so i can stop writing tooling. */
#define TUX64_SRAMDUMPER_FORMAT_MAX_CHARACTERS \
   2048u

struct Tux64SramDumperFormatResult {
   char data [TUX64_SRAMDUMPER_FORMAT_MAX_CHARACTERS];
   Tux64UInt32 characters;
};

/*----------------------------------------------------------------------------*/
/* Formats a stage-0 SRAM dump to human readable text.                        */
/*----------------------------------------------------------------------------*/
struct Tux64SramDumperFormatResult
tux64_sramdumper_format(
   const struct Tux64SramDumperParseInformation * dump
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_SRAMDUMPER_FORMAT_H */

