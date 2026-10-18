/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/interrupt.c - Interrupt service routine         */
/*    implementation.                                                         */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/interrupt.h"

#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/n64/mmio.h>
#include <tux64/platform/mips/n64/mi.h>
#include <tux64/platform/mips/vr4300/cop0.h>
#include <tux64/endian.h>
#include <tux64/bitwise.h>
#include "tux64-boot/halt.h"
#include "tux64-boot/idle.h"
#include "tux64-boot/flag.h"
#include "tux64-boot/cache.h"
#include "tux64-boot/stage1/video.h"
#include "tux64-boot/stage1/audio.h"

/* we store the desired MI_MASK value in a seperate register because we can't */
/* simply read/modify/write the register directly, as reading corresponds to */
/* a different operation than writing, resulting in a corrupted mask.  we */
/* then have to apply our mask all at once. */
static Tux64UInt32
tux64_boot_stage1_interrupt_mi_mask = TUX64_LITERAL_UINT32(0u);

static void
tux64_boot_stage1_interrupt_clear_vi(void) {
   tux64_platform_mips_n64_mmio_registers_vi.v_current = TUX64_LITERAL_UINT32(0u);
   return;
}

static void
tux64_boot_stage1_interrupt_clear_ai(void) {
   tux64_platform_mips_n64_mmio_registers_ai.status = TUX64_LITERAL_UINT32(0u);
   return;
}

static void
tux64_boot_stage1_interrupt_handler_vi(void) {
   tux64_boot_stage1_video_vblank_handler();
   tux64_boot_stage1_interrupt_clear_vi();
   return;
}

static void
tux64_boot_stage1_interrupt_handler_ai(void) {
   tux64_boot_stage1_audio_queue_buffer();
   tux64_boot_stage1_interrupt_clear_ai();
   return;
}

static Tux64Boolean
tux64_boot_stage1_interrupt_handler_mi(void) {
   Tux64UInt32 mi_interrupt;
   Tux64UInt32 interrupt_handled;

   mi_interrupt = tux64_platform_mips_n64_mmio_registers_mi.interrupt;
   interrupt_handled = TUX64_BOOLEAN_FALSE;

   if (tux64_bitwise_flags_check_one_uint32(
      mi_interrupt,
      TUX64_PLATFORM_MIPS_N64_MI_INTERRUPT_BIT_VI
   )) {
      tux64_boot_stage1_interrupt_handler_vi();
      interrupt_handled |= TUX64_BOOLEAN_TRUE;
   }
   if (tux64_boot_flag_audio_beeps() && tux64_bitwise_flags_check_one_uint32(
      mi_interrupt,
      TUX64_PLATFORM_MIPS_N64_MI_INTERRUPT_BIT_AI
   )) {
      tux64_boot_stage1_interrupt_handler_ai();
      interrupt_handled |= TUX64_BOOLEAN_TRUE;
   }
   
   return interrupt_handled;
}

static void
tux64_boot_stage1_interrupt_handler_unhandled(void) {
   if (TUX64_BOOT_CONFIG_DEBUG) {
      /* we should never have unhandled interrupts with correct code, but we */
      /* should still be made aware of them in debug builds. */
      tux64_boot_halt();
   }

   TUX64_UNREACHABLE;
}

/* this is the function which is immediately jumped to by the processor. */
/* thus, we need to tell GCC to generate an appropriate prologue/epilogue. */
/* also don't re-enable interrupts, we don't want that. */
static void
tux64_boot_stage1_interrupt_handler(void)
__attribute__((interrupt, keep_interrupts_masked));

static void
tux64_boot_stage1_interrupt_handler(void) {
   Tux64UInt32 cause;
   Tux64Boolean interrupt_handled;

   tux64_boot_idle_push();

   cause = tux64_platform_mips_vr4300_cop0_register_read_cause();
   interrupt_handled = TUX64_BOOLEAN_FALSE;

   if (tux64_bitwise_flags_check_one_uint32(
      cause,
      TUX64_PLATFORM_MIPS_VR4300_COP0_CAUSE_BIT_IP2
   )) {
      interrupt_handled |= tux64_boot_stage1_interrupt_handler_mi();
   }

   if (interrupt_handled == TUX64_BOOLEAN_FALSE) {
      tux64_boot_stage1_interrupt_handler_unhandled();
   }

   tux64_boot_idle_pop();

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
   Tux64UInt64 * service_routine_address_uncached;
   Tux64UInt64 * service_routine_address_cached;
   Tux64UInt64 service_routine_code;

   jump_target.function = tux64_boot_stage1_interrupt_handler;

   /* we use the uncached segment because since we are writing data, this */
   /* write is invisible to icache.  therefore, we would have to first write */
   /* back dcache, then invalidate icache.  it's easier to just write to the */
   /* uncached segment to eliminate the first cache operation. */
   service_routine_address_uncached = (Tux64UInt64 *)TUX64_LITERAL_UINTPTR(
      TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_UNCACHED
   );
   service_routine_address_cached = (Tux64UInt64 *)TUX64_LITERAL_UINTPTR(
      TUX64_BOOT_STAGE1_INTERRUPT_SERVICE_ROUTINE_ADDRESS_CACHED
   );

   /* insert the address of the service handler into the machine code */
   service_routine_code = tux64_boot_stage1_interrupt_assemble_service_routine_code(
      jump_target.data
   );

   /* correct for endianess...aaaand boom! just like magic! */
   *service_routine_address_uncached = tux64_endian_convert_uint64(
      service_routine_code,
      TUX64_ENDIAN_FORMAT_BIG
   );

   /* see documentation above. */
   tux64_boot_cache_invalidate_instruction(
      service_routine_address_cached,
      TUX64_LITERAL_UINT32(sizeof(service_routine_code))
   );

   return;
}

#define TUX64_BOOT_STAGE1_INTERRUPT_STATUS_BITS \
   ( \
      TUX64_PLATFORM_MIPS_VR4300_COP0_STATUS_BIT_IE | \
      TUX64_PLATFORM_MIPS_VR4300_COP0_STATUS_BIT_IM2 \
   )

static void
tux64_boot_stage1_interrupt_disable_status(
   Tux64UInt32 status
) {
   tux64_platform_mips_vr4300_cop0_register_write_status(
      tux64_bitwise_flags_clear_uint32(
         status,
         TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_INTERRUPT_STATUS_BITS)
      )
   );
   return;
}

static void
tux64_boot_stage1_interrupt_enable_status(
   Tux64UInt32 status
) {
   tux64_platform_mips_vr4300_cop0_register_write_status(
      tux64_bitwise_flags_set_uint32(
         status,
         TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE1_INTERRUPT_STATUS_BITS)
      )
   );
   return;
}

void
tux64_boot_stage1_interrupt_initialize(void) {
   Tux64UInt32 status;

   status = tux64_platform_mips_vr4300_cop0_register_read_status();
   
   tux64_boot_stage1_interrupt_disable_status(status);
   tux64_boot_stage1_interrupt_initialize_handler();

   return;
}

void
tux64_boot_stage1_interrupt_enable(void) {
   Tux64UInt32 status;

   status = tux64_platform_mips_vr4300_cop0_register_read_status();
   
   tux64_boot_stage1_interrupt_enable_status(status);
   return;
}

void
tux64_boot_stage1_interrupt_disable(void) {
   Tux64UInt32 status;

   status = tux64_platform_mips_vr4300_cop0_register_read_status();
   
   tux64_boot_stage1_interrupt_disable_status(status);
   return;
}

void
tux64_boot_stage1_interrupt_rcp_update_mask(void) {
   tux64_platform_mips_n64_mmio_registers_mi.mask = tux64_boot_stage1_interrupt_mi_mask;
   return;
}

void
tux64_boot_stage1_interrupt_vi_enable(void) {
   tux64_boot_stage1_interrupt_mi_mask = tux64_bitwise_flags_set_uint32(
      tux64_boot_stage1_interrupt_mi_mask,
      TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_SET_VI
   );

   return;
}

void
tux64_boot_stage1_interrupt_vi_disable(void) {
   tux64_boot_stage1_interrupt_mi_mask = tux64_bitwise_flags_set_uint32(
      tux64_boot_stage1_interrupt_mi_mask,
      TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_CLEAR_VI
   );

   return;
}

void
tux64_boot_stage1_interrupt_ai_enable(void) {
   tux64_boot_stage1_interrupt_mi_mask = tux64_bitwise_flags_set_uint32(
      tux64_boot_stage1_interrupt_mi_mask,
      TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_SET_AI
   );

   return;
}

void
tux64_boot_stage1_interrupt_ai_disable(void) {
   tux64_boot_stage1_interrupt_mi_mask = tux64_bitwise_flags_set_uint32(
      tux64_boot_stage1_interrupt_mi_mask,
      TUX64_PLATFORM_MIPS_N64_MI_MASK_BIT_CLEAR_AI
   );

   return;
}

