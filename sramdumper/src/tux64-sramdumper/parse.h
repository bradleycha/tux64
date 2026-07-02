/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-sramdumper/parse.h - Header for SRAM dump parser.          */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_SRAMDUMPER_PARSE_H
#define _TUX64_SRAMDUMPER_PARSE_H
/*----------------------------------------------------------------------------*/

#include "tux64-sramdumper/tux64-sramdumper.h"

#define TUX64_SRAMDUMPER_PARSE_INFORMATION_REGISTER_COUNT \
   32u

#define TUX64_SRAMDUMPER_PARSE_INFORMATION_STATUS_CODE_INVALID \
   '\x7f'

/* if the status code is invalid, 'stage' will be set to  */
/* TUX64_SRAMDUMPER_PARSE_INFORMATION_STATUS_CODE_INVALID. */
struct Tux64SramDumperParseInformationStatusCode {
   char stage;
   char code;
};

struct Tux64SramDumperParseInformationStatusCodes {
   struct Tux64SramDumperParseInformationStatusCode last_written;
   struct Tux64SramDumperParseInformationStatusCode time_of_dump;
};

struct Tux64SramDumperParseInformation {
   Tux64UInt32 register_file [TUX64_SRAMDUMPER_PARSE_INFORMATION_REGISTER_COUNT];
   struct Tux64SramDumperParseInformationStatusCodes status_codes;
};

enum Tux64SramDumperParseStatus {
   TUX64_SRAMDUMPER_PARSE_STATUS_OK,
   TUX64_SRAMDUMPER_PARSE_STATUS_INVALID_DUMP
};

union Tux64SramDumperParsePayload {
   struct Tux64SramDumperParseInformation ok;
};

struct Tux64SramDumperParseResult {
   enum Tux64SramDumperParseStatus status;
   union Tux64SramDumperParsePayload payload;
};

/*----------------------------------------------------------------------------*/
/* Attempts to parse an SRAM save dump file and extract the tux64-boot        */
/* stage-0 status codes and register file.                                    */
/*----------------------------------------------------------------------------*/
struct Tux64SramDumperParseResult
tux64_sramdumper_parse(
   const Tux64UInt8 * sram_ptr,
   Tux64UInt32 sram_bytes
);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_SRAMDUMPER_PARSE_H */

