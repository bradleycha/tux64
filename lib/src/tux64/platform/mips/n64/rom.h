/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/n64/rom.h - Cartridge ROM types and constants. */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_ROM_H
#define _TUX64_PLATFORM_MIPS_N64_ROM_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"

#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_PI_DOM1_CONFIGURATION_RESERVED_BYTE\
   (0x80u)

struct Tux64PlatformMipsN64RomHeaderPiDom1Configuration {
   Tux64UInt8 reserved_00;
   Tux64UInt8 flags[3];
};

#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_COUNT\
   (4u)

#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_UNSPECIFIED\
   (0x00u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_RUMBLE_PAK\
   (0x01u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_CONTROLLER_PAK\
   (0x02u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_TRANSFER_PAK\
   (0x03u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_MOUSE\
   (0x80u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_VRU\
   (0x81u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_GAMECUBE_CONTROLLER\
   (0x82u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_RANDNET_KEYBOARD\
   (0x83u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_GAMECUBE_KEYBOARD\
   (0x84u)

#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_GAME_ID_CHARACTERS\
   (2u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_GAME_ID_MAGIC\
   "ED"

#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_NONE\
   (0u << 4u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_EEPROM_4K\
   (1u << 4u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_EEPROM_16K\
   (2u << 4u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_SRAM_256K\
   (3u << 4u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_SRAM_768K\
   (4u << 4u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_FLASH_RAM\
   (5u << 4u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_SRAM_1M\
   (6u << 4u)

#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_BIT_SERIAL_RTC\
   (1u << 0u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_SAVE_TYPE_BIT_REGION_FREE\
   (1u << 1u)

struct Tux64PlatformMipsN64RomHeaderHomebrewInfo {
   Tux64UInt8 controller_type [TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_CONTROLLER_TYPE_COUNT];
   Tux64UInt8 game_id [TUX64_PLATFORM_MIPS_N64_ROM_HEADER_HOMEBREW_INFO_GAME_ID_CHARACTERS];
   Tux64UInt8 save_type;
};

#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_LIBULTRA_VERSION_BYTES\
   (4u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_TITLE_CHARACTERS\
   (20u)
#define TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_CODE_CHARACTERS\
   (4u)

struct Tux64PlatformMipsN64RomHeader {
   struct Tux64PlatformMipsN64RomHeaderPiDom1Configuration pi_dom1_configuration;
   Tux64UInt32 clock_rate;
   Tux64UInt32 boot_address;
   Tux64UInt8 libultra_version [TUX64_PLATFORM_MIPS_N64_ROM_HEADER_LIBULTRA_VERSION_BYTES];
   Tux64UInt64 check_code;
   Tux64UInt8 reserved_18 [8];
   Tux64UInt8 game_title [TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_TITLE_CHARACTERS];
   struct Tux64PlatformMipsN64RomHeaderHomebrewInfo homebrew_info;
   Tux64UInt8 game_code [TUX64_PLATFORM_MIPS_N64_ROM_HEADER_GAME_CODE_CHARACTERS];
   Tux64UInt8 rom_version;
};

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_ROM_H */

