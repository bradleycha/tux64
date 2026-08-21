/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/layout.h - Definitions for the addresses and lengths   */
/*    of manually-defined structures and sections.                            */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_LAYOUT_H
#define _TUX64_BOOT_LAYOUT_H
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include <tux64/math.h>
#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/n64/boot.h>

/*----------------------------------------------------------------------------*/
/* status.h memory layout.                                                    */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_LAYOUT_STATUS_BYTES\
   (8)
#define TUX64_BOOT_LAYOUT_STATUS_ADDRESS\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_DMEM\
      + 0x1000\
      - TUX64_BOOT_LAYOUT_STATUS_BYTES\
   )

/*----------------------------------------------------------------------------*/
/* header.h memory layout.                                                    */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_LAYOUT_HEADER_ADDRESS_OFFSET\
   (0x200) /* place right after exception vectors, with a little padding for safety. */
#define TUX64_BOOT_LAYOUT_HEADER_ADDRESS\
   TUX64_MATH_ALIGN_FORWARD(\
      (\
         TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED\
         + TUX64_BOOT_LAYOUT_HEADER_ADDRESS_OFFSET\
      ),\
      8\
   ) /* aligned for use with RSP DMA. */
#define TUX64_BOOT_LAYOUT_HEADER_BYTES\
   (TUX64_PLATFORM_MIPS_N64_BOOT_HEADER_BYTES)

/*----------------------------------------------------------------------------*/
/* load.h memory layout.                                                      */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_LAYOUT_LOAD_ALLOCATIONS_ADDRESS\
   TUX64_MATH_ALIGN_FORWARD(\
      (\
         TUX64_BOOT_LAYOUT_HEADER_ADDRESS\
         + TUX64_BOOT_LAYOUT_HEADER_BYTES\
      ),\
      4\
   ) /* TODO: compute alignment using AC_CHECK_ALIGNOF(...) */
   
#define TUX64_BOOT_LAYOUT_LOAD_ALLOCATIONS_BYTES\
   (0x0c) /* TODO: generate this using AC_CHECK_SIZEOF(...) */

/*----------------------------------------------------------------------------*/
/* exec.h memory layout.                                                      */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_LAYOUT_EXEC_KERNEL_ARGUMENTS_ADDRESS\
   TUX64_MATH_ALIGN_FORWARD(\
      (\
         TUX64_BOOT_LAYOUT_LOAD_ALLOCATIONS_ADDRESS\
         + TUX64_BOOT_LAYOUT_LOAD_ALLOCATIONS_BYTES\
      ),\
      4\
   ) /* TODO: compute alignment using AC_CHECK_ALIGNOF(...) */
#define TUX64_BOOT_LAYOUT_EXEC_KERNEL_ARGUMENTS_BYTES\
   (0x10) /* TODO: generate this using AC_CHECK_SIZEOF(...) */

/*----------------------------------------------------------------------------*/
/* Stage-2 bootloader memory layout.                                          */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_LAYOUT_STAGE2_STACK_ADDRESS\
   TUX64_MATH_ALIGN_FORWARD(\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED,\
      8\
   ) /* aligned for maximum primitive type (8 bytes) */
#define TUX64_BOOT_LAYOUT_STAGE2_STACK_BYTES\
   TUX64_BOOT_LAYOUT_HEADER_ADDRESS_OFFSET
   /* since we don't need exception processing in stage-2, we can make use of */
   /* the reserved chunk of memory before the boot header for the stack, thus */
   /* freeing more memory for the boot files. */
#define TUX64_BOOT_LAYOUT_STAGE2_LOAD_ADDRESS\
   TUX64_MATH_ALIGN_FORWARD(\
      (\
         TUX64_BOOT_LAYOUT_EXEC_KERNEL_ARGUMENTS_ADDRESS\
         + TUX64_BOOT_LAYOUT_EXEC_KERNEL_ARGUMENTS_BYTES\
      ),\
      8\
   ) /* aligned for use with RSP DMA. */
#define TUX64_BOOT_LAYOUT_STAGE2_LOAD_BYTES_MAXIMUM\
   (0x4000) /* 4KiB, matches the maximum size we used to have with RSP memory */
#define TUX64_BOOT_LAYOUT_STAGE2_MEMORY_RESERVED\
   (\
      TUX64_BOOT_LAYOUT_STAGE2_LOAD_ADDRESS\
      + TUX64_BOOT_LAYOUT_STAGE2_LOAD_BYTES_MAXIMUM\
   )

/*----------------------------------------------------------------------------*/
/* Stage-1 bootloader memory layout.  Declared after stage-2 memory layout    */
/* due to dependencies.                                                       */
/*----------------------------------------------------------------------------*/
#define TUX64_BOOT_LAYOUT_STAGE1_STACK_ADDRESS\
   TUX64_MATH_ALIGN_FORWARD(\
      (\
         TUX64_BOOT_LAYOUT_STAGE2_LOAD_ADDRESS\
         + TUX64_BOOT_LAYOUT_STAGE2_LOAD_BYTES_MAXIMUM\
      ),\
      8\
   ) /* aligned for maximum primitive type (8 bytes) */
#define TUX64_BOOT_LAYOUT_STAGE1_STACK_BYTES\
   (0x00001000) /* 4KiB stack, should be plenty */
#define TUX64_BOOT_LAYOUT_STAGE1_LOAD_ADDRESS\
   TUX64_MATH_ALIGN_FORWARD(\
      (\
         TUX64_BOOT_LAYOUT_STAGE1_STACK_ADDRESS\
         + TUX64_BOOT_LAYOUT_STAGE1_STACK_BYTES\
      ),\
      8\
   ) /* aligned for use with RSP DMA. */
#define TUX64_BOOT_LAYOUT_STAGE1_MEMORY_RESERVED\
   (\
      TUX64_BOOT_LAYOUT_STAGE1_LOAD_ADDRESS\
      - TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED\
   )

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_LAYOUT_H */

