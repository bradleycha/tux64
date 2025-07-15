/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/vr4300/cache.h - Header for VR4300 cache       */
/*    management functions.                                                   */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_PLATFORM_MIPS_VR4300_CACHE_H
#define _TUX64_PLATFORM_MIPS_VR4300_CACHE_H
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/vr4300/vr4300.h"

#if TUX64_PLATFORM_MIPS_VR4300_ENABLE
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* The number of bytes per line of cache.                                     */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_INSTRUCTION\
   (32u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_BYTES_PER_LINE_DATA\
   (16u)

/*----------------------------------------------------------------------------*/
/* The total number of cache lines.                                           */
/*----------------------------------------------------------------------------*/
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_LINE_COUNT_INSTRUCTION\
   (512u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_LINE_COUNT_DATA\
   (512u)

/*----------------------------------------------------------------------------*/
/* Low-level wrappers for the cache instruction.  These may have side-effects */
/* on co-processor 0, or may also required co-processor 0 registers to be set */
/* before use.  These are essentially directly executing the 'cache'          */
/* instruction verbatim according to the VR4300 manual.  'address' must point */
/* to a memory region which supports cache operations.                        */
/*----------------------------------------------------------------------------*/
void
tux64_platform_mips_vr4300_cache_operation_instruction_index_invalidate(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_instruction_index_load_tag(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_instruction_index_store_tag(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_instruction_hit_invalidate(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_instruction_fill(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_instruction_hit_write_back(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_data_index_write_back_invalidate(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_data_index_load_tag(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_data_index_store_tag(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_data_create_dirty_exclusive(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_data_hit_invalidate(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_data_hit_write_back_invalidate(
   const void * address
);
void
tux64_platform_mips_vr4300_cache_operation_data_hit_write_back(
   const void * address
);

/*----------------------------------------------------------------------------*/
#endif /* TUX64_PLATFORM_MIPS_VR4300_ENABLE */

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_PLATFORM_MIPS_VR4300_CACHE_H */

