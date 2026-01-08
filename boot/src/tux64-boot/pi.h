/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/pi.h - Header for controlling the Peripheral Interface */
/*    (PI).                                                                   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_PI_H
#define _TUX64_BOOT_PI_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/pi.h>

/*----------------------------------------------------------------------------*/
/* Describes a PI DMA transfer operation.                                     */
/*----------------------------------------------------------------------------*/
/* addr_pibus -                                                               */
/*    The address with respect to the PI address space to start the transfer  */
/*    at.  This must be aligned to 2 bytes.                                   */
/*                                                                            */
/* addr_rdram -                                                               */
/*    The physical address in RDRAM to start the transfer at.  This must be   */
/*    aligned to 8 bytes.  This can either be a physical, uncached, or cached */
/*    virtual address.                                                        */
/*                                                                            */
/* bytes -                                                                    */
/*    The number of bytes to transfer, minus one.  This must be a multiple of */
/*    2.                                                                      */
/*----------------------------------------------------------------------------*/
struct Tux64BootPiDmaTransfer {
   Tux64PlatformMipsN64PiBusAddress addr_pibus;
   Tux64UInt32 addr_rdram;
   Tux64UInt32 bytes;
};

enum Tux64BootPiDmaDestination {
   TUX64_BOOT_PI_DMA_DESTINATION_PIBUS,
   TUX64_BOOT_PI_DMA_DESTINATION_RDRAM
};

/*----------------------------------------------------------------------------*/
/* Starts a PI DMA transfer asynchronously.  The PI Bus must not be in the    */
/* middle of either an ongoing DMA transfer or memory-mapped read or write    */
/* when this function is called.                                              */
/*----------------------------------------------------------------------------*/
void
tux64_boot_pi_dma_start(
   const struct Tux64BootPiDmaTransfer * transfer,
   enum Tux64BootPiDmaDestination destination
);

enum Tux64BootPiDmaStatus {
   TUX64_BOOT_PI_DMA_STATUS_COMPLETED,
   TUX64_BOOT_PI_DMA_STATUS_IN_PROGRESS,
   TUX64_BOOT_PI_DMA_STATUS_IO_ERROR
};

/*----------------------------------------------------------------------------*/
/* Gets the status of the current DMA operation.                              */
/*----------------------------------------------------------------------------*/
enum Tux64BootPiDmaStatus
tux64_boot_pi_dma_status(void);

/*----------------------------------------------------------------------------*/
/* Idles the CPU until the current DMA transfer either completes or an I/O    */
/* error is encountered.  Returns the last read status of the DMA operation.  */
/* Will never return TUX64_BOOT_PI_DMA_STATUS_IN_PROGRESS.                    */
/*----------------------------------------------------------------------------*/
enum Tux64BootPiDmaStatus
tux64_boot_pi_dma_wait_idle(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_PI_H */

