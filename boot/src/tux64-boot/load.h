/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/load.h - Header for file load allocations and          */
/*    progress, shared between stage-1 and stage-2.                           */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_LOAD_H
#define _TUX64_BOOT_LOAD_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"

/*----------------------------------------------------------------------------*/
/* Stores bitflags representing if each file is loaded by stage-1 or stage-2. */
/* If a bit it set to '1', it is loaded by stage-1.  If a bit is set to '0',  */
/* it is loaded by stage-2.                                                   */
/*----------------------------------------------------------------------------*/
typedef Tux64UInt8 Tux64BootLoadStatus;

/*----------------------------------------------------------------------------*/
/* Bitflags representing the load status of each individual file.             */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_LOAD_STATUS_KERNEL \
   (1u << 0u)
#define TUX64_BOOT_LOAD_STATUS_INITRAMFS \
   (1u << 1u)
#define TUX64_BOOT_LOAD_STATUS_COMMAND_LINE \
   (1u << 2u)

/*----------------------------------------------------------------------------*/
/* When the load status is exactly equal to this constant, it means every     */
/* file could be loaded from stage-1, thus there is no need for stage-2.      */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_LOAD_STATUS_CONCURRENT_WITH_STAGE1 \
   ( \
      TUX64_BOOT_LOAD_STATUS_KERNEL       | \
      TUX64_BOOT_LOAD_STATUS_INITRAMFS    | \
      TUX64_BOOT_LOAD_STATUS_COMMAND_LINE \
   )

/*----------------------------------------------------------------------------*/
/* Stores the allocated region in RDRAM for a single file.  If the file is    */
/* empty, then 'address' should be zero.                                      */
/*----------------------------------------------------------------------------*/
struct Tux64BootLoadAllocationsFile {
   Tux64UInt32 address;
};

struct Tux64BootLoadAllocationsRequired {
   struct Tux64BootLoadAllocationsFile kernel;
};

struct Tux64BootLoadAllocationsOptional {
   struct Tux64BootLoadAllocationsFile initramfs;
   struct Tux64BootLoadAllocationsFile command_line;
};

/*----------------------------------------------------------------------------*/
/* Stores the allocations for every boot file.  All files in 'required' must  */
/* be present, thus will never be empty.  All files in 'optional' may be      */
/* absent, thus may be empty.                                                 */
/*----------------------------------------------------------------------------*/
struct Tux64BootLoadAllocations {
   struct Tux64BootLoadAllocationsRequired required;
   struct Tux64BootLoadAllocationsOptional optional;
};

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_LOAD_H */

