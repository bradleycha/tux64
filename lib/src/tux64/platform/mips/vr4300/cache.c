/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* lib/src/tux64/platform/mips/vr4300/cache.c - Implementations for VR4300    */
/*    cache management functions.                                             */
/*----------------------------------------------------------------------------*/

#include "tux64/tux64.h"
#include "tux64/platform/mips/vr4300/cache.h"

#include "tux64/platform/mips/vr4300/vr4300.h"

#if TUX64_PLATFORM_MIPS_VR4300_ENABLE
/*----------------------------------------------------------------------------*/

#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_INSTRUCTION\
   (0x00u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA\
   (0x01u)

#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_INVALIDATE\
   (0x00u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_WRITE_BACK_INVALIDATE\
   (0x00u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_LOAD_TAG\
   (0x01u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_STORE_TAG\
   (0x02u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_CREATE_DIRTY_EXCLUSIVE\
   (0x03u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_INVALIDATE\
   (0x04u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_WRITE_BACK_INVALIDATE\
   (0x05u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_FILL\
   (0x05u)
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_WRITE_BACK\
   (0x06u)

#define TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(target, type)\
   (target) | ((type) << 2u)

/* This is ridiculously cracked, but it's required to get efficient code-gen. */
/* in particular, imagine the following: we want to perform an operation on a */
/* compile-time known address.  if we were to use the "r" constraint, we      */
/* would get the following:                                                   */
/*                                                                            */
/* lui   $t0,HI(addr)                                                         */
/* ori   $t0,$t0,LO(addr)                                                     */
/* cache op,0($t0)                                                            */
/*                                                                            */
/* We instead use the MIPS-specific "ZC" constraint, which will allow the     */
/* compiler to use the base-offset format for us, if appropriate.  Now, the   */
/* above code turns into the following:                                       */
/*                                                                            */
/* lui   $t0,HI(addr)                                                         */
/* cache op,LO(addr)($t0)                                                     */
/*                                                                            */
/* This saves one instruction per invocation of the cache instruction.  Now,  */
/* the syntax for 'address' look ridiculous...and that's because it is!  For  */
/* some reason, we have to dereference the pointer type, which then the       */
/* compiler gets the address of the address for us.  It's stupid, but it      */
/* works.                                                                     */
/*                                                                            */
/* Also, we use 'volatile' because since we have no outputs, the compiler may */
/* remove the cache instruction entirely under heavy optimizations due to it  */
/* not detecting any use of the asm statement.                                */
#define TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(address, operation)\
   __asm__ volatile ( \
      "cache   %0,%1"\
      :: "K" (operation), "ZC" (*((const Tux64UInt8 *)(address)))\
   );

void
tux64_platform_mips_vr4300_cache_operation_instruction_index_invalidate(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_INSTRUCTION,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_INVALIDATE
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_instruction_index_load_tag(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_INSTRUCTION,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_LOAD_TAG
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_instruction_index_store_tag(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_INSTRUCTION,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_STORE_TAG
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_instruction_hit_invalidate(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_INSTRUCTION,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_INVALIDATE
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_instruction_fill(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_INSTRUCTION,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_FILL
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_instruction_hit_write_back(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_INSTRUCTION,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_WRITE_BACK
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_data_index_write_back_invalidate(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_WRITE_BACK_INVALIDATE
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_data_index_load_tag(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_LOAD_TAG
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_data_index_store_tag(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_INDEX_STORE_TAG
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_data_create_dirty_exclusive(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_CREATE_DIRTY_EXCLUSIVE
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_data_hit_invalidate(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_INVALIDATE
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_data_hit_write_back_invalidate(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_WRITE_BACK_INVALIDATE
      ))
   );

   return;
}

void
tux64_platform_mips_vr4300_cache_operation_data_hit_write_back(
   const void * address
) {
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      address,
      TUX64_LITERAL_UINT16(TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION(
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TARGET_DATA,
         TUX64_PLATFORM_MIPS_VR4300_CACHE_OPERATION_TYPE_HIT_WRITE_BACK
      ))
   );

   return;
}

/*----------------------------------------------------------------------------*/
#endif /* TUX64_PLATFORM_MIPS_VR4300_ENABLE */

