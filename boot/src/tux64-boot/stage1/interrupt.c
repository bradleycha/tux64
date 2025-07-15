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

/* we use inline asm so we don't have to use a seperate file, but also so we */
/* can automate struct sizes and offsets using assembler templates stuff. */
/* it's ugly due to having everything in a string literal, but it's the best */
/* way of doing it. */
void
tux64_boot_stage1_interrupt_entry(void);
__asm__ (
   "# disable syntactic sugar so we have complete control\n"
   ".set noreorder\n"
   ".set noat\n"
   ".set nomacro\n"
   "\n"
   ".extern tux64_boot_stage1_interrupt_handler\n"
   "\n"
   "   .section .text\n"
   "   .global tux64_boot_stage1_interrupt_entry\n"
   "tux64_boot_stage1_interrupt_entry:\n"
   "   # make room for the context struct\n"
   "   addiu $sp,$sp,-%[context_bytes]\n"
   "\n"
   "   # populate context struct with register file\n"
   "   sd    $zero,(0*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $at,( 1*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $v0,( 2*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $v1,( 3*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $a0,( 4*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $a1,( 5*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $a2,( 6*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $a3,( 7*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t0,( 8*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t1,( 9*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t2,(10*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t3,(11*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t4,(12*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t5,(13*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t6,(14*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t7,(15*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $s0,(16*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $s1,(17*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $s2,(18*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $s3,(19*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $s4,(20*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $s5,(21*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $s6,(22*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $s7,(23*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t8,(24*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t9,(25*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $k0,(26*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $k1,(27*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "\n"
   "   # prepare a couple of special registers which require a little bit of\n"
   "   # elbow grease to read.  we already preserved the temp registers, so\n"
   "   # we can freely use these.\n"
   "   addiu $t0,$sp,%[context_bytes] # $t0 = context stack pointer\n"
   "   mfc0  $t1,$" TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_EPC " # $t1 = context program counter\n"
   "   mflo  $t2 # $t3 = context LO register\n"
   "   mfhi  $t3 # $t4 = context HI register\n"
   "\n"
   "   # continue storing the rest of the register file\n"
   "   sd    $gp,(28*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t0,(29*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $fp,(30*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $ra,(31*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t1,(32*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t2,(33*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   sd    $t3,(34*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "\n"
   "   # run the interrupt handler with the context struct\n"
   "   jal   tux64_boot_stage1_interrupt_handler\n"
   "   addiu $a0,$sp,0\n"
   "\n"
   "   # restore the register file from the context struct, since the\n"
   "   # interrupt handler may modify any of the registers (ex: GDB).  note\n"
   "   # that we save $sp for last, as that messes with the code.  otherwise,\n"
   "   # it's just the reverse of the above.\n"
   "   ld    $t3,(34*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t2,(33*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t1,(32*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $ra,(31*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $fp,(30*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $gp,(28*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   mthi  $t3\n"
   "   mtlo  $t2\n"
   "   mtc0  $t1,$" TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_EPC "\n"
   "   ld    $k1,(27*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $k0,(26*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t9,(25*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t8,(24*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $s7,(23*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $s6,(22*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $s5,(21*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $s4,(20*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $s3,(19*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $s2,(18*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $s1,(17*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $s0,(16*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t7,(15*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t6,(14*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t5,(13*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t4,(12*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t3,(11*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t2,(10*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t1,( 9*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $t0,( 8*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $a3,( 7*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $a2,( 6*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $a1,( 5*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $a0,( 4*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $v1,( 3*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $v0,( 2*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $at,( 1*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   ld    $zero,(0*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "\n"
   "   # finally, restore the stack and return from interrupt\n"
   "   ld    $sp,(29*%[context_register_bytes] + %[context_offsetof_register_file])($sp)\n"
   "   eret\n"
   "#tux64_boot_stage1_interrupt_entry\n"
   :: [context_bytes]                  "K" TUX64_LITERAL_UINT16(sizeof(struct Tux64BootStage1InterruptContext)),
      [context_offsetof_register_file] "K" TUX64_LITERAL_UINT16(TUX64_OFFSET_OF(struct Tux64BootStage1InterruptContext, register_file)),
      [context_register_bytes]         "K" TUX64_LITERAL_UINT16(sizeof(Tux64BootStage1InterruptContextRegister))
);

/* this must be public so we can access it from the assembly above */
void
tux64_boot_stage1_interrupt_handler(
   struct Tux64BootStage1InterruptContext * context
) {
   /* TODO: implement */
   context->register_file.pc.uint += 4;
   (void)context;
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

