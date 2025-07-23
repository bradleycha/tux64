/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2025                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/interrupt/interrupt.c - Interrupt service       */
/*    routine implementation.                                                 */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/interrupt/interrupt.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/n64/mmio.h>
#include <tux64/platform/mips/n64/mi.h>
#include <tux64/platform/mips/vr4300/cop0.h>
#include <tux64/platform/mips/vr4300/cache.h>
#include <tux64/endian.h>
#include <tux64/bitwise.h>
#include "tux64-boot/stage1/interrupt/entry.h"
#include "tux64-boot/stage1/halt.h"
#include "tux64-boot/stage1/video.h"

static void
tux64_boot_stage1_interrupt_handler_vi(
   struct Tux64BootStage1InterruptContext * context
) {
   tux64_boot_stage1_video_vblank_handler();

   /* clears the VI interrupt */
   tux64_platform_mips_n64_mmio_registers_vi.v_current = TUX64_LITERAL_UINT32(0u);

   (void)context;
   return;
}

static void
tux64_boot_stage1_interrupt_handler_mi(
   struct Tux64BootStage1InterruptContext * context
) {
   Tux64UInt32 mi_interrupt;

   mi_interrupt = tux64_platform_mips_n64_mmio_registers_mi.interrupt;

   if (tux64_bitwise_flags_check_one_uint32(
      mi_interrupt,
      TUX64_PLATFORM_MIPS_N64_MI_INTERRUPT_BIT_VI
   )) {
      tux64_boot_stage1_interrupt_handler_vi(context);
      return;
   }
   
   (void)context;
   return;
}

static void
tux64_boot_stage1_interrupt_handler_unhandled(
   struct Tux64BootStage1InterruptContext * context
) {
   (void)context;
   tux64_boot_stage1_halt();
   TUX64_UNREACHABLE;
}

/* this must be public so we can access it from the assembly above */
void
tux64_boot_stage1_interrupt_handler(
   struct Tux64BootStage1InterruptContext * context
) {
   Tux64UInt32 cause;
   Tux64Boolean interrupt_handled;

   cause = tux64_platform_mips_vr4300_cop0_register_read_cause();
   interrupt_handled = TUX64_BOOLEAN_FALSE;

   if (tux64_bitwise_flags_check_one_uint32(
      cause,
      TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP2
   )) {
      tux64_boot_stage1_interrupt_handler_mi(context);
      interrupt_handled = TUX64_BOOLEAN_TRUE;
   }

   if (interrupt_handled == TUX64_BOOLEAN_FALSE) {
      tux64_boot_stage1_interrupt_handler_unhandled(context);
   }

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
      /* j 0x00000000   */ ((0x08000000llu) << 32u) | \
      /* nop            */ ((0x00000000llu) <<  0u)   \
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

   jump_target.function = tux64_boot_stage1_interrupt_entry;

   /* we use the uncached address because we have to invalidate instruction */
   /* cache anyways, so there's no point in caching the write in data cache. */
   /* in fact, that could cause further issues that can be easily avoided. */
   service_routine_address = (Tux64UInt64 *)TUX64_LITERAL_UINTPTR(
      TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_UNCACHED
   );

   /* insert the address of the service handler into the machine code */
   service_routine_code = tux64_boot_stage1_interrupt_assemble_service_routine_code(
      jump_target.data
   );

   /* correct for endianess...aaaand boom! just like magic! */
   *service_routine_address = tux64_endian_convert_uint64(
      service_routine_code,
      TUX64_ENDIAN_FORMAT_BIG
   );

   /* don't forget to tell the CPU what we just did, as instruction cache */
   /* isn't aware of what's going in in data cache (which we aren't using */
   /* anyways) */
   tux64_platform_mips_vr4300_cache_operation_instruction_hit_invalidate(
      (const void *)TUX64_LITERAL_UINTPTR(
         TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_CACHED
      )
   );

   return;
}

void
tux64_boot_stage1_interrupt_initialize(void) {
   Tux64UInt32 status;

   status = tux64_platform_mips_vr4300_cop0_register_read_status();
   
   /* disable interrupts while we initialize */
   tux64_platform_mips_vr4300_cop0_register_write_status(
      tux64_bitwise_flags_clear_uint32(
         status,
         TUX64_PLATFORM_MIPS_VR4300_COP0_STATUS_BIT_IE
      )
   );

   tux64_boot_stage1_interrupt_initialize_handler();

   /* enable interrupts now that we have the handler initialized.  note that */
   /* individual sub-systems, like the VI, may need to be enabled manually. */
   tux64_platform_mips_vr4300_cop0_register_write_status(
      tux64_bitwise_flags_set_uint32(
         status,
         TUX64_PLATFORM_MIPS_VR4300_COP0_STATUS_BIT_IE
      )
   );

   return;
}

void
tux64_boot_stage1_interrupt_vi_enable(void) {
   tux64_platform_mips_n64_mmio_registers_mi.mask = tux64_bitwise_flags_set_uint32(
      tux64_platform_mips_n64_mmio_registers_mi.mask,
      TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_SET_VI
   );

   return;
}

void
tux64_boot_stage1_interrupt_vi_disable(void) {
   tux64_platform_mips_n64_mmio_registers_mi.mask = tux64_bitwise_flags_set_uint32(
      tux64_platform_mips_n64_mmio_registers_mi.mask,
      TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_CLEAR_VI
   );

   return;
}

