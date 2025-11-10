/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/rsp.h - Header for controlling the RSP.         */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STAGE1_RSP_H
#define _TUX64_BOOT_STAGE1_RSP_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Describes an RSP DMA transfer operation.                                   */
/*----------------------------------------------------------------------------*/
/* addr_rsp_mem -                                                             */
/*    The address in RSP memory to start the transfer at.  This must be       */
/*    aligned to 8 bytes.  This can either be a physical, uncached virtual,   */
/*    or cached virtual address.                                              */
/*                                                                            */
/* addr_rdram -                                                               */
/*    The physical address in RDRAM to start the transfer at.  This must be   */
/*    aligned to 8 bytes.  This can either be a physical, uncached virtual,   */
/*    or cached virtual address.                                              */
/*                                                                            */
/* row_bytes_copy -                                                           */
/*    The amount of bytes per row to copy, starting at the beginning of the   */
/*    row.  This has a maximum value of (2^12 - 1).                           */
/*                                                                            */
/* row_bytes_skip -                                                           */
/*    The amount of bytes per row to skip over and get to the next row,       */
/*    starting after the copied bytes.  This has a maximum value of           */
/*    (2^12 - 1).                                                             */
/*                                                                            */
/* row_count -                                                                */
/*    The number of rows to copy, minus one.  For example, to copy a single   */
/*    row, this would be '0'.  To copy 16 lines, this would be '15'.          */
/*----------------------------------------------------------------------------*/
struct Tux64BootStage1RspDmaTransfer {
   Tux64UInt32 addr_rsp_mem;
   Tux64UInt32 addr_rdram;
   Tux64UInt16 row_bytes_copy;
   Tux64UInt16 row_bytes_skip;
   Tux64UInt8  row_count;
};

enum Tux64BootStage1RspDmaDestination {
   TUX64_BOOT_STAGE1_RSP_DMA_DESTINATION_RSP_MEMORY,
   TUX64_BOOT_STAGE1_RSP_DMA_DESTINATION_RDRAM
};

/*----------------------------------------------------------------------------*/
/* Starts an RSP DMA transfer asynchronously.  The RSP must not be in the     */
/* middle of an ongoing DMA transfer when this function is called.            */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_rsp_dma_start(
   const struct Tux64BootStage1RspDmaTransfer * transfer,
   enum Tux64BootStage1RspDmaDestination destination
);

/*----------------------------------------------------------------------------*/
/* Waits for the RSP to complete all queued and active DMA transfers.         */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_rsp_dma_wait_idle(void);

/*----------------------------------------------------------------------------*/
/* Waits for the RSP to start its queued DMA transfer, allowing a new DMA     */
/* transfer to be queued.                                                     */
/*----------------------------------------------------------------------------*/
void
tux64_boot_stage1_rsp_dma_wait_queue(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STAGE1_RSP_H */

