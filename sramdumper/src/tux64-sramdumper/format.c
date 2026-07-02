/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* mkrom/src/tux64-sramdumper/format.c - Implementations for SRAM dump        */
/*    formatter.                                                              */
/*----------------------------------------------------------------------------*/

#include "tux64-sramdumper/tux64-sramdumper.h"
#include "tux64-sramdumper/format.h"

#include <tux64/memory.h>
#include "tux64-sramdumper/parse.h"

#include <stdio.h>
#include <inttypes.h>

static Tux64UInt32
tux64_sramdumper_format_status_code(
   char * buffer,
   const struct Tux64SramDumperParseInformationStatusCode * status,
   const char * label
) {
   Tux64UInt32 characters;
   char * ptr;
   Tux64UInt32 len;

   characters = TUX64_LITERAL_UINT32(0u);
   ptr = buffer;

   len = (Tux64UInt32)sprintf(ptr, "   status code (%s): ", label);
   ptr += len;
   characters += len;

   if (status->stage == TUX64_SRAMDUMPER_PARSE_INFORMATION_STATUS_CODE_INVALID) {
      len = (Tux64UInt32)sprintf(ptr, "(invalid)");
   } else {
      len = (Tux64UInt32)sprintf(ptr, "stage %c, code %c", status->stage, status->code);
   }
   ptr += len;
   characters += len;

   len = (Tux64UInt32)sprintf(ptr, "\n");
   ptr += len;
   characters += len;

   return characters;
}

#define TUX64_SRAMDUMPER_FORMAT_REGISTERS_PER_ROW \
   4u
#define TUX64_SRAMDUMPER_FORMAT_REGISTER_NAME_LENGTH \
   2u

TUX64_ASSERT_STATIC((TUX64_SRAMDUMPER_PARSE_INFORMATION_REGISTER_COUNT % TUX64_SRAMDUMPER_FORMAT_REGISTERS_PER_ROW) == 0u);

static const char
tux64_sramdumper_format_register_names [TUX64_SRAMDUMPER_PARSE_INFORMATION_REGISTER_COUNT][TUX64_SRAMDUMPER_FORMAT_REGISTER_NAME_LENGTH] = {
   {'z', 'r'},
   {'a', 't'},
   {'v', '0'},
   {'v', '1'},
   {'a', '0'},
   {'a', '1'},
   {'a', '2'},
   {'a', '3'},
   {'t', '0'},
   {'t', '1'},
   {'t', '2'},
   {'t', '3'},
   {'t', '4'},
   {'t', '5'},
   {'t', '6'},
   {'t', '7'},
   {'s', '0'},
   {'s', '1'},
   {'s', '2'},
   {'s', '3'},
   {'s', '4'},
   {'s', '5'},
   {'s', '6'},
   {'s', '7'},
   {'t', '8'},
   {'t', '9'},
   {'k', '0'},
   {'k', '1'},
   {'g', 'p'},
   {'s', 'p'},
   {'f', 'p'},
   {'r', 'a'}
};

static Tux64UInt32
tux64_sramdumper_format_register_file_row(
   char * buffer,
   const Tux64UInt32 registers [TUX64_SRAMDUMPER_FORMAT_REGISTERS_PER_ROW],
   const char names [TUX64_SRAMDUMPER_FORMAT_REGISTERS_PER_ROW][TUX64_SRAMDUMPER_FORMAT_REGISTER_NAME_LENGTH],
   Tux64UInt8 register_base_index
) {
   Tux64UInt32 characters;
   char * ptr;
   Tux64UInt32 len;
   Tux64UInt8 i;

   characters = TUX64_LITERAL_UINT32(0u);
   ptr = buffer;

   len = (Tux64UInt32)sprintf(ptr, "  ");
   ptr += len;
   characters += len;

   i = TUX64_LITERAL_UINT8(0u);
   do {
      len = (Tux64UInt32)sprintf(
         ptr,
         "           $%.*s",
         TUX64_SRAMDUMPER_FORMAT_REGISTER_NAME_LENGTH,
         names[i]
      );
      ptr += len;
      characters += len;

      i++;
   } while (i != TUX64_LITERAL_UINT8(TUX64_SRAMDUMPER_FORMAT_REGISTERS_PER_ROW));

   len = (Tux64UInt32)sprintf(ptr, "\n     ");
   ptr += len;
   characters += len;

   i = TUX64_LITERAL_UINT8(0u);
   do {
      len = (Tux64UInt32)sprintf(
         ptr,
         "    %10" PRIu32,
         registers[i]
      );
      ptr += len;
      characters += len;

      i++;
   } while (i != TUX64_LITERAL_UINT8(TUX64_SRAMDUMPER_FORMAT_REGISTERS_PER_ROW));

   len = (Tux64UInt32)sprintf(ptr, "\n [%2" PRIu8 "]", register_base_index);
   ptr += len;
   characters += len;

   i = TUX64_LITERAL_UINT8(0u);
   do {
      len = (Tux64UInt32)sprintf(
         ptr,
         "    0x%08" PRIx32,
         registers[i]
      );
      ptr += len;
      characters += len;

      i++;
   } while (i != TUX64_LITERAL_UINT8(TUX64_SRAMDUMPER_FORMAT_REGISTERS_PER_ROW));

   len = (Tux64UInt32)sprintf(ptr, "\n\n");
   ptr += len;
   characters += len;

   return characters;
}

static Tux64UInt32
tux64_sramdumper_format_register_file(
   char * buffer,
   const Tux64UInt32 register_file [TUX64_SRAMDUMPER_PARSE_INFORMATION_REGISTER_COUNT]
) {
   Tux64UInt32 characters;
   char * ptr;
   Tux64UInt32 len;
   Tux64UInt8 i;

   characters = TUX64_LITERAL_UINT32(0u);
   ptr = buffer;

   i = TUX64_LITERAL_UINT8(0u);
   do {
      len = tux64_sramdumper_format_register_file_row(
         ptr,
         &register_file[i],
         &tux64_sramdumper_format_register_names[i],
         i
      );
      ptr += len;
      characters += len;

      i += TUX64_LITERAL_UINT8(TUX64_SRAMDUMPER_FORMAT_REGISTERS_PER_ROW);
   } while (i != TUX64_LITERAL_UINT8(TUX64_SRAMDUMPER_PARSE_INFORMATION_REGISTER_COUNT));

   return characters;
}

struct Tux64SramDumperFormatResult
tux64_sramdumper_format(
   const struct Tux64SramDumperParseInformation * dump
) {
   struct Tux64SramDumperFormatResult result;
   char * ptr;
   Tux64UInt32 len;

   result.characters = TUX64_LITERAL_UINT32(0u);
   ptr = result.data;

   len = (Tux64UInt32)sprintf(ptr, "--------------------------------- STATUS CODES ---------------------------------\n\n");
   ptr += len;
   result.characters += len;

   len = tux64_sramdumper_format_status_code(ptr, &dump->status_codes.last_written, "last written");
   ptr += len;
   result.characters += len;

   len = tux64_sramdumper_format_status_code(ptr, &dump->status_codes.time_of_dump, "time of dump");
   ptr += len;
   result.characters += len;

   len = (Tux64UInt32)sprintf(ptr, "\n------------------------- GENERAL PURPOSE REGISTER FILE ------------------------\n\n");
   ptr += len;
   result.characters += len;

   len = tux64_sramdumper_format_register_file(ptr, dump->register_file);
   ptr += len;
   result.characters += len;
   
   return result;
}

