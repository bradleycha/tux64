/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* init/src/tux64-init/system-info.c - Implementations for system info        */
/*    querying.                                                               */
/*----------------------------------------------------------------------------*/

#include "tux64-init/tux64-init.h"
#include "tux64-init/system-info.h"

#include <tux64/log.h>
#include <tux64/platform/mips/n64/boot.h>
#include <tux64/platform/mips/n64/ipl2.h>
#include <tux64/platform/mips/n64/procfs.h>

#include <inttypes.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>

static void
tux64_init_system_info_print_separator(void) {
   TUX64_LOG_INFO("--------------------------------------------");
   return;
}

static void
tux64_init_system_info_print_kernel(void) {
   struct utsname kernel;

   /* uname() is only documented to error with EFAULT if 'buf' is not valid. */
   /* since we know 'buf' is always valid, we can safely ignore errors here. */
   (void)uname(&kernel);

   TUX64_LOG_INFO_FMT(
      "Running on %s/%s version %s",
      kernel.sysname,
      kernel.machine,
      kernel.release
   );

   return;
}

#define TUX64_INIT_SYSTEM_INFO_N64_PROCFS_ENABLE\
   (\
      TUX64_INIT_CONFIG_PROCFS &&\
      TUX64_PLATFORM_MIPS_N64_PROCFS_ENABLE\
   )

#if TUX64_INIT_SYSTEM_INFO_N64_PROCFS_ENABLE
/*----------------------------------------------------------------------------*/

union Tux64InitSystemInfoN64ProcfsEntry {
   enum Tux64PlatformMipsN64BootConsoleType console_type;
   enum Tux64PlatformMipsN64Ipl2RomType rom_type;
   enum Tux64PlatformMipsN64Ipl2ResetType reset_type;
   Tux64UInt8 rom_cic_seed;
   Tux64UInt8 version_pif_rom;
   Tux64UInt8 version_rsp;
   Tux64UInt8 version_rdp;
   Tux64UInt8 version_rac;
   Tux64UInt8 version_io;
};

static void
tux64_init_system_info_print_n64_procfs_string(
   const char * name,
   const char * value
) {
   TUX64_LOG_INFO_FMT("%s | %s", name, value);
   return;
}

static void
tux64_init_system_info_print_n64_procfs_hex(
   const char * name,
   Tux64UInt8 value
) {
   TUX64_LOG_INFO_FMT("%s | 0x%02" PRIx8, name, value);
   return;
}

static void
tux64_init_system_info_print_n64_procfs_byte(
   const char * name,
   Tux64UInt8 value
) {
   TUX64_LOG_INFO_FMT("%s | %" PRIu8, name, value);
   return;
}

static const char *
tux64_init_system_info_format_n64_procfs_console_type(
   enum Tux64PlatformMipsN64BootConsoleType console_type
) {
   const char * string;

   switch (console_type) {
      case TUX64_PLATFORM_MIPS_N64_BOOT_CONSOLE_TYPE_PAL:
         string = "PAL N64 console";
         break;
      case TUX64_PLATFORM_MIPS_N64_BOOT_CONSOLE_TYPE_NTSC:
         string = "NTSC N64 console";
         break;
      case TUX64_PLATFORM_MIPS_N64_BOOT_CONSOLE_TYPE_MPAL:
         string = "M-PAL N64 console";
         break;
      case TUX64_PLATFORM_MIPS_N64_BOOT_CONSOLE_TYPE_IQUE:
         string = "iQue player";
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return string;
}

static const char *
tux64_init_system_info_format_n64_procfs_rom_type(
   enum Tux64PlatformMipsN64Ipl2RomType rom_type
) {
   const char * string;

   switch (rom_type) {
      case TUX64_PLATFORM_MIPS_N64_IPL2_ROM_TYPE_CART:
         string = "Game pak";
         break;
      case TUX64_PLATFORM_MIPS_N64_IPL2_ROM_TYPE_64DD:
         string = "64DD diskette";
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return string;
}

static const char *
tux64_init_system_info_format_n64_procfs_reset_type(
   enum Tux64PlatformMipsN64Ipl2ResetType reset_type
) {
   const char * string;

   switch (reset_type) {
      case TUX64_PLATFORM_MIPS_N64_IPL2_RESET_TYPE_HARD:
         string = "Hard reset";
         break;
      case TUX64_PLATFORM_MIPS_N64_IPL2_RESET_TYPE_SOFT:
         string = "Soft reset";
         break;
      default:
         TUX64_UNREACHABLE;
   }

   return string;
}

static void
tux64_init_system_info_print_n64_procfs(void) {
   enum Tux64PlatformMipsN64ProcfsReadStatus read_status;
   union Tux64InitSystemInfoN64ProcfsEntry entry;
   const char * string;

   read_status = tux64_platform_mips_n64_procfs_read_console_type(&entry.console_type);
   if (read_status == TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      string = tux64_init_system_info_format_n64_procfs_console_type(entry.console_type);
      tux64_init_system_info_print_n64_procfs_string("Console type   ", string);
   }

   read_status = tux64_platform_mips_n64_procfs_read_rom_type(&entry.rom_type);
   if (read_status == TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      string = tux64_init_system_info_format_n64_procfs_rom_type(entry.rom_type);
      tux64_init_system_info_print_n64_procfs_string("ROM type       ", string);
   }

   read_status = tux64_platform_mips_n64_procfs_read_reset_type(&entry.reset_type);
   if (read_status == TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      string = tux64_init_system_info_format_n64_procfs_reset_type(entry.reset_type);
      tux64_init_system_info_print_n64_procfs_string("Reset type     ", string);
   }

   read_status = tux64_platform_mips_n64_procfs_read_rom_cic_seed(&entry.rom_cic_seed);
   if (read_status == TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      tux64_init_system_info_print_n64_procfs_hex("ROM CIC seed   ", entry.rom_cic_seed);
   }

   read_status = tux64_platform_mips_n64_procfs_read_version_pif_rom(&entry.version_pif_rom);
   if (read_status == TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      tux64_init_system_info_print_n64_procfs_byte("PIF ROM version", entry.version_pif_rom);
   }

   read_status = tux64_platform_mips_n64_procfs_read_version_rsp(&entry.version_rsp);
   if (read_status == TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      tux64_init_system_info_print_n64_procfs_byte("RSP version    ", entry.version_rsp);
   }

   read_status = tux64_platform_mips_n64_procfs_read_version_rdp(&entry.version_rdp);
   if (read_status == TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      tux64_init_system_info_print_n64_procfs_byte("RDP version    ", entry.version_rdp);
   }

   read_status = tux64_platform_mips_n64_procfs_read_version_rac(&entry.version_rac);
   if (read_status == TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      tux64_init_system_info_print_n64_procfs_byte("RAC version    ", entry.version_rac);
   }

   read_status = tux64_platform_mips_n64_procfs_read_version_io(&entry.version_io);
   if (read_status == TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      tux64_init_system_info_print_n64_procfs_byte("I/O version    ", entry.version_io);
   }

   return;
}

/*----------------------------------------------------------------------------*/
#else /* TUX64_INIT_SYSTEM_INFO_N64_PROCFS_ENABLE */
/*----------------------------------------------------------------------------*/

static void
tux64_init_system_info_print_n64_procfs(void) {
   return;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_INIT_SYSTEM_INFO_N64_PROCFS_ENABLE */

static void
tux64_init_system_info_print_memory_statistic(
   const char * name,
   Tux64UInt32 bytes
) {
   Tux64UInt32 mib_hi;
   Tux64UInt32 mib_lo;

   mib_hi = bytes >> TUX64_LITERAL_UINT8(20u);
   mib_lo = (bytes - (mib_hi << TUX64_LITERAL_UINT8(20u))) >> TUX64_LITERAL_UINT8(10u);

   TUX64_LOG_INFO_FMT(
      "%s | %" PRIu32 ".%03" PRIu32 " MiB (%" PRIu32 " bytes)",
      name, mib_hi, mib_lo, bytes
   );

   return;
}

static void
tux64_init_system_info_print_memory(void) {
   struct sysinfo mem;

   /* we can ignore the error here for the same reason as with uname(). */
   (void)sysinfo(&mem);

   tux64_init_system_info_print_memory_statistic("Memory total   ", mem.totalram);
   tux64_init_system_info_print_memory_statistic("Memory free    ", mem.freeram);
   tux64_init_system_info_print_memory_statistic("Memory shared  ", mem.sharedram);
   tux64_init_system_info_print_memory_statistic("Memory buffered", mem.bufferram);

   return;
}

void
tux64_init_system_info_print(void) {
   tux64_init_system_info_print_separator();
   tux64_init_system_info_print_kernel();

   tux64_init_system_info_print_separator();
   tux64_init_system_info_print_n64_procfs();

   tux64_init_system_info_print_separator();
   tux64_init_system_info_print_memory();

   tux64_init_system_info_print_separator();
   return;
}

