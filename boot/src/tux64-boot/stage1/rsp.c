/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/rsp.c - Implementations for controlling the     */
/*    RSP.                                                                    */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/rsp.h"

#include <tux64/platform/mips/n64/mmio.h>
#include "tux64-boot/stage1/idle.h"

static Tux64UInt32
tux64_boot_stage1_rsp_dma_encode_length(
   Tux64UInt16 row_bytes_copy,
   Tux64UInt16 row_bytes_skip,
   Tux64UInt8 row_count
) {
   Tux64UInt32 retn;

   retn = TUX64_LITERAL_UINT32(0u);
   retn = retn | (Tux64UInt32)(row_bytes_copy   << TUX64_LITERAL_UINT8( 0u));
   retn = retn | (Tux64UInt32)(row_count        << TUX64_LITERAL_UINT8(12u));
   retn = retn | (Tux64UInt32)(row_bytes_skip   << TUX64_LITERAL_UINT8(20u));

   return retn;
}

void
tux64_boot_stage1_rsp_dma_start(
   const struct Tux64BootStage1RspDmaTransfer * transfer,
   enum Tux64BootStage1RspDmaDestination destination
) {
   volatile Tux64UInt32 * reg;
   Tux64UInt32 length;

   switch (destination) {
      case TUX64_BOOT_STAGE1_RSP_DMA_DESTINATION_RSP_MEMORY:
         reg = &tux64_platform_mips_n64_mmio_registers_sp.dma_rdlen;
         break;
      case TUX64_BOOT_STAGE1_RSP_DMA_DESTINATION_RDRAM:
         reg = &tux64_platform_mips_n64_mmio_registers_sp.dma_wrlen;
         break;
      default:
         TUX64_UNREACHABLE;
   }

   length = tux64_boot_stage1_rsp_dma_encode_length(
      transfer->row_bytes_copy,
      transfer->row_bytes_skip,
      transfer->row_count
   );

   /* upper bits which encode the virtual address are ignored, so we can just */
   /* directly write these. */
   tux64_platform_mips_n64_mmio_registers_sp.dma_spaddr  = transfer->addr_rsp_mem;
   tux64_platform_mips_n64_mmio_registers_sp.dma_ramaddr = transfer->addr_rdram;

   /* writing to this register starts the DMA operations, or queues it up if */
   /* there is already a DMA operation in progress. */
   *reg = length;

   return;
}

static void
tux64_boot_stage1_rsp_dma_wait_spinlock(
   const volatile Tux64UInt32 * reg
) {
   Tux64UInt32 busy;

   tux64_boot_stage1_idle_enter();
   do {
      busy = *reg;
   } while (busy != TUX64_LITERAL_UINT32(0u));
   tux64_boot_stage1_idle_exit();

   return;
}

void
tux64_boot_stage1_rsp_dma_wait_idle(void) {
   tux64_boot_stage1_rsp_dma_wait_spinlock(
      &tux64_platform_mips_n64_mmio_registers_sp.dma_busy
   );
   return;
}

void
tux64_boot_stage1_rsp_dma_wait_queue(void) {
   tux64_boot_stage1_rsp_dma_wait_spinlock(
      &tux64_platform_mips_n64_mmio_registers_sp.dma_full
   );
   return;
}

