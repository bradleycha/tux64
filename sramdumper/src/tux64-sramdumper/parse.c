/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-sramdumper/parse.c - Implementations for SRAM dump parser. */
/*----------------------------------------------------------------------------*/

#include "tux64-sramdumper/tux64-sramdumper.h"
#include "tux64-sramdumper/parse.h"

#include <tux64/memory.h>
#include <tux64/endian.h>
#include <tux64/string.h>

#define TUX64_SRAMDUMPER_PARSE_ENDIAN_FORMAT_N64 \
   TUX64_ENDIAN_FORMAT_BIG

#define TUX64_SRAMDUMPER_PARSE_STATUS_CODE_BYTES \
   8u
#define TUX64_SRAMDUMPER_PARSE_DUMP_MINIMUM_SIZE \
   ( \
      ((TUX64_SRAMDUMPER_PARSE_STATUS_CODE_BYTES) * 2u) + \
      (sizeof(Tux64UInt32) * TUX64_SRAMDUMPER_PARSE_INFORMATION_REGISTER_COUNT) \
   )

#define TUX64_SRAMDUMPER_PARSE_OFFSET_STATUS_CODE_LAST_WRITTEN \
   0u
#define TUX64_SRAMDUMPER_PARSE_OFFSET_STATUS_CODE_TIME_OF_DUMP \
   (TUX64_SRAMDUMPER_PARSE_OFFSET_STATUS_CODE_LAST_WRITTEN + TUX64_SRAMDUMPER_PARSE_STATUS_CODE_BYTES)
#define TUX64_SRAMDUMPER_PARSE_OFFSET_REGISTER_FILE \
   (TUX64_SRAMDUMPER_PARSE_OFFSET_STATUS_CODE_TIME_OF_DUMP + TUX64_SRAMDUMPER_PARSE_STATUS_CODE_BYTES)

#define TUX64_SRAMDUMPER_PARSE_STATUS_CODE_PREFIX \
   "STAGE"

static struct Tux64SramDumperParseInformationStatusCode
tux64_sramdumper_parse_status_code(
   const char bytes [TUX64_SRAMDUMPER_PARSE_STATUS_CODE_BYTES]
) {
   struct Tux64SramDumperParseInformationStatusCode status;

   if (tux64_memory_compare_with_equal_lengths(
      &bytes[0u],
      TUX64_SRAMDUMPER_PARSE_STATUS_CODE_PREFIX,
      TUX64_LITERAL_UINT32(TUX64_STRING_CHARACTERS(TUX64_SRAMDUMPER_PARSE_STATUS_CODE_PREFIX))
   ) == TUX64_BOOLEAN_FALSE) {
      goto invalid_status;
   }

   if (bytes[6u] != ':') {
      goto invalid_status;
   }

   status.stage  = bytes[5u];
   status.code   = bytes[7u];

   if (tux64_string_character_is_numeric(status.stage) == TUX64_BOOLEAN_FALSE) {
      goto invalid_status;
   }
   if (tux64_string_character_is_alphabetic_uppercase(status.code) == TUX64_BOOLEAN_FALSE) {
      goto invalid_status;
   }

   return status;

invalid_status:
   status.stage = TUX64_SRAMDUMPER_PARSE_INFORMATION_STATUS_CODE_INVALID;
   return status;
}

static void
tux64_sramdumper_parse_correct_register_file_endianess(
   Tux64UInt32 register_file [TUX64_SRAMDUMPER_PARSE_INFORMATION_REGISTER_COUNT]
) {
   Tux64UInt8 i;

   i = TUX64_LITERAL_UINT8(TUX64_SRAMDUMPER_PARSE_INFORMATION_REGISTER_COUNT);
   do {
      i--;

      register_file[i] = tux64_endian_convert_uint32(
         register_file[i],
         TUX64_SRAMDUMPER_PARSE_ENDIAN_FORMAT_N64
      );
   } while (i != TUX64_LITERAL_UINT8(0u));

   return;
}

struct Tux64SramDumperParseResult
tux64_sramdumper_parse(
   const Tux64UInt8 * sram_ptr,
   Tux64UInt32 sram_bytes
) {
   struct Tux64SramDumperParseResult result;
   char status_buffer [TUX64_SRAMDUMPER_PARSE_STATUS_CODE_BYTES];

   if (sram_bytes < TUX64_LITERAL_UINT32(TUX64_SRAMDUMPER_PARSE_DUMP_MINIMUM_SIZE)) {
      result.status = TUX64_SRAMDUMPER_PARSE_STATUS_INVALID_DUMP;
      return result;
   }

   tux64_memory_copy(
      status_buffer,
      &sram_ptr[TUX64_SRAMDUMPER_PARSE_OFFSET_STATUS_CODE_LAST_WRITTEN],
      TUX64_LITERAL_UINT32(sizeof(status_buffer))
   );
   result.payload.ok.status_codes.last_written = tux64_sramdumper_parse_status_code(status_buffer);
   tux64_memory_copy(
      status_buffer,
      &sram_ptr[TUX64_SRAMDUMPER_PARSE_OFFSET_STATUS_CODE_TIME_OF_DUMP],
      TUX64_LITERAL_UINT32(sizeof(status_buffer))
   );
   result.payload.ok.status_codes.time_of_dump = tux64_sramdumper_parse_status_code(status_buffer);

   tux64_memory_copy(
      result.payload.ok.register_file,
      &sram_ptr[TUX64_SRAMDUMPER_PARSE_OFFSET_REGISTER_FILE],
      TUX64_LITERAL_UINT32(sizeof(result.payload.ok.register_file))
   );
   tux64_sramdumper_parse_correct_register_file_endianess(result.payload.ok.register_file);

   result.status = TUX64_SRAMDUMPER_PARSE_STATUS_OK;
   return result;
}

