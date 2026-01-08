/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/pi.h - Implementations for controlling the Peripheral  */
/*    Interface (PI).                                                         */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/pi.h"

#include <tux64/platform/mips/n64/pi.h>
#include <tux64/platform/mips/n64/mmio.h>
#include <tux64/bitwise.h>
#include "tux64-boot/idle.h"
#include "tux64-boot/sync.h"

void
tux64_boot_pi_dma_start(
   const struct Tux64BootPiDmaTransfer * transfer,
   enum Tux64BootPiDmaDestination destination
) {
   volatile Tux64UInt32 * reg;

   switch (destination) {
      case TUX64_BOOT_PI_DMA_DESTINATION_PIBUS:
         reg = &tux64_platform_mips_n64_mmio_registers_pi.rd_len;
         break;

      case TUX64_BOOT_PI_DMA_DESTINATION_RDRAM:
         reg = &tux64_platform_mips_n64_mmio_registers_pi.wr_len;
         break;

      default:
         TUX64_UNREACHABLE;
   }

   tux64_platform_mips_n64_mmio_registers_pi.dram_addr = transfer->addr_rdram;
   tux64_platform_mips_n64_mmio_registers_pi.cart_addr = transfer->addr_pibus;

   /* writing to this register starts the DMA transfer. */
   *reg = transfer->bytes;

   return;
}

enum Tux64BootPiDmaStatus
tux64_boot_pi_dma_status(void) {
   Tux64UInt32 status;

   /* read this once to prevent race conditions. */
   status = tux64_platform_mips_n64_mmio_registers_pi.status;

   if (tux64_bitwise_flags_check_one_uint32(
      status,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_PI_STATUS_BIT_DMA_BUSY)
   )) {
      return TUX64_BOOT_PI_DMA_STATUS_IN_PROGRESS;
   }
   if (tux64_bitwise_flags_check_one_uint32(
      status,
      TUX64_LITERAL_UINT32(TUX64_PLATFORM_MIPS_N64_PI_STATUS_BIT_DMA_ERROR)
   )) {
      return TUX64_BOOT_PI_DMA_STATUS_IO_ERROR;
   }

   return TUX64_BOOT_PI_DMA_STATUS_COMPLETED;
}

enum Tux64BootPiDmaStatus
tux64_boot_pi_dma_wait_idle(void) {
   enum Tux64BootPiDmaStatus status;

   tux64_boot_idle_enter();
   do {
      status = tux64_boot_pi_dma_status();
   } while (status != TUX64_BOOT_PI_DMA_STATUS_COMPLETED);
   tux64_boot_idle_exit();

   return status;
}

