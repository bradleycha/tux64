/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* develop/headers/platform-n64/rom.h - Nintendo 64 cartridge ROM interfaces. */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_N64_ROM_H
#define _TUX64_PLATFORM_N64_ROM_H
/*------------------------------------------------------------------------------*/

#include "tux64/tux64.h"

struct Tux64PlatformN64RomHeaderPiBsdDom1Configuration {
   Tux64UInt8
   reserved_00;

   Tux64UInt8
   flags_00;

   Tux64UInt8
   flags_01;
};

#define TUX64_PLATFORM_N64_ROM_HEADER_HOMEBREW_INFORMATION_CONTROLLER_TYPE_COUNT\
   (4u)
#define TUX64_PLATFORM_N64_ROM_HEADER_HOMEBREW_INFORMATION_GAME_ID_LENGTH\
   (2u)

struct Tux64PlatformN64RomHeaderHomebrewInformation {
   Tux64UInt8
   controller_type [TUX64_PLATFORM_N64_ROM_HEADER_HOMEBREW_INFORMATION_CONTROLLER_TYPE_COUNT];

   Tux64UInt8
   game_id [TUX64_PLATFORM_N64_ROM_HEADER_HOMEBREW_INFORMATION_GAME_ID_LENGTH];

   Tux64UInt8
   save_type;
};

#define TUX64_PLATFORM_N64_ROM_HEADER_LIBULTRA_VERSION_LENGTH\
   (4u)
#define TUX64_PLATFORM_N64_ROM_HEADER_GAME_TITLE_LENGTH\
   (20u)
#define TUX64_PLATFORM_N64_ROM_HEADER_GAME_CODE_LENGTH\
   (4u)

struct Tux64PlatformN64RomHeader {
   struct Tux64PlatformN64RomHeaderPiBsdDom1Configuration
   pi_bsd_dom1_configuration;

   Tux64UInt32
   clock_rate;

   Tux64UInt32
   boot_address;

   Tux64UInt8
   libultra_version [TUX64_PLATFORM_N64_ROM_HEADER_LIBULTRA_VERSION_LENGTH];

   Tux64UInt64
   check_code;

   Tux64UInt64
   reserved_18;

   Tux64UInt8
   game_title [TUX64_PLATFORM_N64_ROM_HEADER_GAME_TITLE_LENGTH];

   struct Tux64PlatformN64RomHeaderHomebrewInformation
   homebrew_information;

   Tux64UInt8
   game_code [TUX64_PLATFORM_N64_ROM_HEADER_GAME_CODE_LENGTH];

   Tux64UInt8
   rom_version;
};

/*------------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_N64_ROM_H */

