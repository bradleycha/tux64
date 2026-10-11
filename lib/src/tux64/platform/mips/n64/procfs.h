/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64-lib/platform/mips/n64/procfs.h - Header for N64 proc         */
/*    filesystem wrappers.                                                    */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_N64_PROCFS_H
#define _TUX64_PLATFORM_MIPS_N64_PROCFS_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/n64/boot.h"
#include "tux64/platform/mips/n64/ipl2.h"

/*----------------------------------------------------------------------------*/
/* Determines whether the current configuration supports the N64's proc       */
/* filesystem or not.  All proc filesystem wrappers depend on this being      */
/* enabled.                                                                   */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_N64_PROCFS_ENABLE\
   (\
      TUX64_CONFIG_PLATFORM_MIPS_N64 && \
      TUX64_CONFIG_PLATFORM_MIPS_N64_PROCFS && \
      TUX64_HAVE_STDLIB_H &&\
      TUX64_HAVE_STDIO_H &&\
      TUX64_HAVE_INTTYPES_H &&\
      TUX64_HAVE_ERRNO_H\
   )

#if TUX64_PLATFORM_MIPS_N64_PROCFS_ENABLE
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* The result of reading a value from the proc filesystem.                    */
/*----------------------------------------------------------------------------*/
enum Tux64PlatformMipsN64ProcfsReadStatus {
   TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK,
   TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OUT_OF_MEMORY,
   TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_NOT_PRESENT,
   TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_IO_ERROR
};

/*----------------------------------------------------------------------------*/
/* Attempts to read various proc filesystem entries.  'output' is only valid  */
/* when TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK is returned.            */
/*----------------------------------------------------------------------------*/
enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_console_type(
   enum Tux64PlatformMipsN64BootConsoleType * output
);
enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_rom_type(
   enum Tux64PlatformMipsN64Ipl2RomType * output
);
enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_reset_type(
   enum Tux64PlatformMipsN64Ipl2ResetType * output
);
enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_rom_cic_seed(
   Tux64UInt8 * output
);
enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_pif_rom(
   Tux64UInt8 * output
);
enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_rsp(
   Tux64UInt8 * output
);
enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_rdp(
   Tux64UInt8 * output
);
enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_rac(
   Tux64UInt8 * output
);
enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_io(
   Tux64UInt8 * output
);

/*----------------------------------------------------------------------------*/
#endif /* TUX64_PLATFORM_MIPS_N64_PROCFS_ENABLE */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_N64_PROCFS_H */

