/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/pif.h - Implementations for PIF interfaces.            */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/pif.h"

#include <tux64/endian.h>
#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/n64/pif.h>

#define TUX64_BOOT_PIF_COMMAND_ADDRESS \
   (TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_PIF_RAM + 0x3cu)

static void
tux64_boot_pif_send_command(
   Tux64UInt8 command
) {
   volatile Tux64UInt32 * cmd;
   Tux64UInt32 word;

   cmd   = (volatile Tux64UInt32 *)TUX64_LITERAL_UINTPTR(TUX64_BOOT_PIF_COMMAND_ADDRESS);
   word  = tux64_endian_convert_uint32((Tux64UInt32)command, TUX64_ENDIAN_FORMAT_BIG);

   /* we use a dummy load to block the CPU until the SI is inactive. */
   (void)(*cmd);

   /* now it's safe to write the command word. */
   *cmd = word;

   return;
}

void
tux64_boot_pif_terminate_boot(void) {
   tux64_boot_pif_send_command(TUX64_LITERAL_UINT8(
      TUX64_PLATFORM_MIPS_N64_PIF_COMMAND_TERMINATE_BOOT
   ));

   return;
}

