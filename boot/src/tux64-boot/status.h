/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/status.h - Boot status code header.                    */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_STATUS_H
#define _TUX64_BOOT_STATUS_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/platform/mips/n64/memory-map.h>

/*----------------------------------------------------------------------------*/
/* The high and low order magic words used for the status code, before        */
/* formatting in the stage and code.                                          */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STATUS_MAGIC_HI \
   (0x53544147u) /* STAG */
#define TUX64_BOOT_STATUS_MAGIC_LO \
   (0x45003a00u) /* Ex:x */

/*----------------------------------------------------------------------------*/
/* The number of bytes used for the status code.                              */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STATUS_BYTES\
   (8)

/*----------------------------------------------------------------------------*/
/* The address of the status code in memory.                                  */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_STATUS_ADDRESS\
   ( \
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM \
      + 0x1000 \
      - TUX64_BOOT_STATUS_BYTES \
   )

#if !TUX64_PREPROCESSOR_ONLY
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Initializes the low half of the status code word, which is static and      */
/* doesn't change based on boot status.                                       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_status_code_initialize(void);

/*----------------------------------------------------------------------------*/
/* Formats and writes the current boot status code high half.  The "stage"    */
/* and "code" should be the ASCII representation of the stage and code.       */
/*----------------------------------------------------------------------------*/
void
tux64_boot_status_code_write(
   char stage,
   char code
);

/*----------------------------------------------------------------------------*/
#endif /* !TUX64_PREPROCESSOR_ONLY */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_STATUS_H */

