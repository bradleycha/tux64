/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/interrupt.c - Interrupt service routine         */
/*    implementations.                                                        */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/interrupt.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/vr4300/cop0.h>
#include <tux64/platform/mips/vr4300/cache.h>
#include <tux64/endian.h>
#include "tux64-boot/stage1/service.h"

/* this must be public so it can be called from service.s */
void
tux64_boot_stage1_interrupt_handler(void) {
   /* TODO: implement */
   return;
}

#define TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_RAM_OFFSET\
   (0x180u)

#define TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_UNCACHED\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_UNCACHED + \
      TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_RAM_OFFSET \
   )
#define TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_CACHED\
   (\
      TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RDRAM_CACHED + \
      TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_RAM_OFFSET \
   )

/* we can encode the entire trampoline as a single 64-bit integer, so that's */
/* what we do.  the jump target will be calculated at runtime due to */
/* the inability to calculate the address of the service routine when */
/* compiling the C source to machine code.  LTO might be able to do this for */
/* us, but I haven't yet tested if that's the case. */
#define TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_CODE\
   (\
      /* j 0x00000000   */ (0x08000000llu) << 32u |   \
      /* nop            */ (0x00000000llu) <<  0u     \
   )

static Tux64UInt64
tux64_boot_stage1_interrupt_assemble_service_routine_code(
   const void * jump_target
) {
   Tux64UInt64 code;
   Tux64UInt32 jump_bits;

   code = TUX64_LITERAL_UINT64(TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_CODE);

   jump_bits = (Tux64UIntPtr)jump_target;
   jump_bits = jump_bits >> TUX64_LITERAL_UINT8(2u);
   jump_bits = jump_bits & TUX64_LITERAL_UINT32(0x03ffffffu);

   code = code | (((Tux64UInt64)jump_bits) << TUX64_LITERAL_UINT8(32u));

   code = tux64_endian_convert_uint64(code, TUX64_ENDIAN_FORMAT_BIG);

   return code;
}

typedef void (*Tux64BootStage1InterruptJumpTargetFunction)(void);

/* used to circumvent issues with converting function pointers to object */
/* pointers */
union Tux64BootStage1InterruptJumpTarget {
   Tux64BootStage1InterruptJumpTargetFunction function;
   const void * data;
};

static void
tux64_boot_stage1_interrupt_initialize_handler(void) {
   union Tux64BootStage1InterruptJumpTarget jump_target;
   Tux64UInt64 * service_routine_address;
   Tux64UInt64 service_routine_code;

   jump_target.function = tux64_boot_stage1_service_exception;

   /* we use the uncached address because we have to invalidate instruction */
   /* cache anyways, so there's no point in caching the write in data cache. */
   /* in fact, that could cause further issues that can be easily avoided. */
   service_routine_address = (Tux64UInt64 *)TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_UNCACHED;

   service_routine_code = tux64_boot_stage1_interrupt_assemble_service_routine_code(
      jump_target.data
   );

   *service_routine_address = service_routine_code;

   /* TODO: wrap this better.  it's better than before, but it's still fugly. */
   tux64_platform_mips_vr4300_cop0_register_write_taglo(TUX64_LITERAL_UINT32(0x00000000));
   tux64_platform_mips_vr4300_cop0_register_write_taghi(TUX64_LITERAL_UINT32(0x00000000));
   TUX64_PLATFORM_MIPS_VR4300_CACHE_INSTRUCTION(
      TUX64_LITERAL_UINTPTR(TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_CACHED),
      TUX64_LITERAL_UINT16(0x0008u) /* store tag index */
   );

   return;
}

void
tux64_boot_stage1_interrupt_initialize(void) {
   tux64_boot_stage1_interrupt_initialize_handler();
   return;
}

void
tux64_boot_stage1_interrupt_enable(void) {
   /* TODO: implement */
   return;
}

void
tux64_boot_stage1_interrupt_disable(void) {
   /* TODO: implement */
   return;
}

