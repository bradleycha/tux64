/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64-lib/platform/mips/n64/procfs.c - Implementations for N64     */
/*    proc filesystem wrappers.                                               */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/n64/procfs.h"

#if TUX64_PLATFORM_MIPS_N64_PROCFS_ENABLE
/*----------------------------------------------------------------------------*/

#include "tux64/platform/mips/n64/boot.h"
#include "tux64/platform/mips/n64/ipl2.h"

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <errno.h>

#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE\
   "/proc/n64/"

#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_CONSOLE_TYPE\
   TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE "console_type"
#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_ROM_TYPE\
   TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE "rom_type"
#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_RESET_TYPE\
   TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE "reset_type"
#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_ROM_CIC_SEED\
   TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE "rom_cic_seed"
#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_PIF_ROM\
   TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE "version/pif_rom"
#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_RSP\
   TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE "version/rsp"
#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_RDP\
   TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE "version/rdp"
#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_RAC\
   TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE "version/rac"
#define TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_IO\
   TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_BASE "version/io"

/*----------------------------------------------------------------------------*/
/* we use a bit of a weird trick here to minimize code.  we have a generic    */
/* procfs read function, but don't export it publically.  we only export it   */
/* here, and then compile the implementation for lib.  we then compile the    */
/* public wrappers for core, and use the private implementation.              */
/*----------------------------------------------------------------------------*/

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_uint8(
   Tux64UInt8 * output,
   const char * path
);

#if TUX64_IMPLEMENTATION_LIB
/*----------------------------------------------------------------------------*/

static enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_errno_to_status(
   int err
) {
   enum Tux64PlatformMipsN64ProcfsReadStatus status;

   switch (err) {
      case 0:
         /* this function should only be called under error cases, so errno */
         /* should never be zero. */
         TUX64_UNREACHABLE;
      case ENOMEM:
         status = TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OUT_OF_MEMORY;
         break;
      case ENOENT:
         status = TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_NOT_PRESENT;
         break;
      default:
         /* some other weird crap, don't worry about it and just give a */
         /* generic error message. */
         status = TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_IO_ERROR;
         break;
   }

   return status;
}


static enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_uint8_file(
   Tux64UInt8 * output,
   FILE * file
) {
   int result;
   unsigned int v;

   result = fscanf(file, "%" PRIx8, &v);
   if (result < 1 || result == EOF) {
      return tux64_platform_mips_n64_procfs_errno_to_status(errno);
   }

   *output = (Tux64UInt8)v;
   return TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK;
}

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_uint8(
   Tux64UInt8 * output,
   const char * path
) {
   enum Tux64PlatformMipsN64ProcfsReadStatus status;
   FILE * file;

   file = fopen(path, "r");
   if (file == NULL) {
      return tux64_platform_mips_n64_procfs_errno_to_status(errno);
   }

   status = tux64_platform_mips_n64_procfs_read_uint8_file(output, file);
   (void)fclose(file);
   return status;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_IMPLEMENTATION_LIB */

#if TUX64_IMPLEMENTATION_CORE
/*----------------------------------------------------------------------------*/

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_console_type(
   enum Tux64PlatformMipsN64BootConsoleType * output
) {
   enum Tux64PlatformMipsN64ProcfsReadStatus status;
   Tux64UInt8 sentinel;

   status = tux64_platform_mips_n64_procfs_read_uint8(
      &sentinel,
      TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_CONSOLE_TYPE
   );
   if (status != TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      return status;
   }

   *output = (enum Tux64PlatformMipsN64BootConsoleType)sentinel;
   return TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK;
}

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_rom_type(
   enum Tux64PlatformMipsN64Ipl2RomType * output
) {
   enum Tux64PlatformMipsN64ProcfsReadStatus status;
   Tux64UInt8 sentinel;

   status = tux64_platform_mips_n64_procfs_read_uint8(
      &sentinel,
      TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_ROM_TYPE
   );
   if (status != TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      return status;
   }

   *output = (enum Tux64PlatformMipsN64Ipl2RomType)sentinel;
   return TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK;
}

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_reset_type(
   enum Tux64PlatformMipsN64Ipl2ResetType * output
) {
   enum Tux64PlatformMipsN64ProcfsReadStatus status;
   Tux64UInt8 sentinel;

   status = tux64_platform_mips_n64_procfs_read_uint8(
      &sentinel,
      TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_RESET_TYPE
   );
   if (status != TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK) {
      return status;
   }

   *output = (enum Tux64PlatformMipsN64Ipl2ResetType)sentinel;
   return TUX64_PLATFORM_MIPS_N64_PROCFS_READ_STATUS_OK;
}

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_rom_cic_seed(
   Tux64UInt8 * output
) {
   return tux64_platform_mips_n64_procfs_read_uint8(
      output,
      TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_ROM_CIC_SEED
   );
}

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_pif_rom(
   Tux64UInt8 * output
) {
   return tux64_platform_mips_n64_procfs_read_uint8(
      output,
      TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_PIF_ROM
   );
}

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_rsp(
   Tux64UInt8 * output
) {
   return tux64_platform_mips_n64_procfs_read_uint8(
      output,
      TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_RSP
   );
}

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_rdp(
   Tux64UInt8 * output
) {
   return tux64_platform_mips_n64_procfs_read_uint8(
      output,
      TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_RDP
   );
}

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_rac(
   Tux64UInt8 * output
) {
   return tux64_platform_mips_n64_procfs_read_uint8(
      output,
      TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_RAC
   );
}

enum Tux64PlatformMipsN64ProcfsReadStatus
tux64_platform_mips_n64_procfs_read_version_io(
   Tux64UInt8 * output
) {
   return tux64_platform_mips_n64_procfs_read_uint8(
      output,
      TUX64_PLATFORM_MIPS_N64_PROCFS_PATH_VERSION_IO
   );
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_IMPLEMENTATION_CORE */

/*----------------------------------------------------------------------------*/
#endif /* TUX64_PLATFORM_MIPS_N64_PROCFS_ENABLE */

