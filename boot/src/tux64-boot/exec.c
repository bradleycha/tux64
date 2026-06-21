/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/exec.c - Implementations for starting the kernel and   */
/*    stage-2.                                                                */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/exec.h"

#include <tux64/endian.h>
#include <tux64/platform/mips/n64/memory-map.h>
#include "tux64-boot/load.h"
#include "tux64-boot/stage2/stack.h"

void
tux64_boot_exec_kernel(
   const void * entrypoint,
   const struct Tux64BootExecKernelArguments * arguments
) {
   Tux64UInt32 fw_arg0_u32;
   Tux64UInt32 fw_arg1_u32;
   Tux64UInt32 fw_arg2_u32;
   Tux64UInt32 fw_arg3_u32;
   unsigned long fw_arg0;
   unsigned long fw_arg1;
   unsigned long fw_arg2;
   unsigned long fw_arg3;

   fw_arg0_u32 = (Tux64UIntPtr)arguments;
   fw_arg1_u32 = TUX64_LITERAL_UINT32(0u);
   fw_arg2_u32 = TUX64_LITERAL_UINT32(0u);
   fw_arg3_u32 = TUX64_LITERAL_UINT32(0u);

   /* theoretically the kernel could be a foreign endianess, so we do this. */
   fw_arg0 = tux64_endian_convert_uint32(fw_arg0_u32, TUX64_ENDIAN_FORMAT_BIG);
   fw_arg1 = tux64_endian_convert_uint32(fw_arg1_u32, TUX64_ENDIAN_FORMAT_BIG);
   fw_arg2 = tux64_endian_convert_uint32(fw_arg2_u32, TUX64_ENDIAN_FORMAT_BIG);
   fw_arg3 = tux64_endian_convert_uint32(fw_arg3_u32, TUX64_ENDIAN_FORMAT_BIG);

   /* need to write asm explicitly so compiler doesn't bugger up the jump. */
   /* also need .set noreorder so assembler doesn't insert useless nops. */
   __asm__ volatile (
      ".set noreorder\n"
      "jr %0\n"
      "nop\n"
      :: "r"      (entrypoint),
         "{a0}"   (fw_arg0),
         "{a1}"   (fw_arg1),
         "{a2}"   (fw_arg2),
         "{a3}"   (fw_arg3)
   );
   TUX64_UNREACHABLE;
}

/* needs to be a macro so it's evaluated as a constant for use with the "K" */
/* assembler template constraint. */
#define TUX64_BOOT_EXEC_STAGE2_STACK_POINTER \
   (TUX64_BOOT_STAGE2_STACK_ADDRESS + TUX64_BOOT_STAGE2_STACK_BYTES)

void
tux64_boot_exec_stage2(
   Tux64UInt32 memory_total,
   Tux64BootLoadStatus load_status
) {
   const void * entrypoint;

   entrypoint = (const void *)TUX64_LITERAL_UINTPTR(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_IMEM);

   /* same issue as above, but now we also have to set the stack pointer. */
   __asm__ volatile (
      ".set noreorder\n"
      "lui $sp,%1\n"
      "jr %0\n"
      "addiu $sp,$sp,%2\n"
      :: "r"      (entrypoint),
         "K"      (TUX64_BOOT_EXEC_STAGE2_STACK_POINTER >> TUX64_LITERAL_UINT8(16u)),
         "K"      (TUX64_BOOT_EXEC_STAGE2_STACK_POINTER & TUX64_LITERAL_UINT32(TUX64_UINT16_MAX)),
         "{s0}"   (memory_total),
         "{s1}"   (load_status)
   );
   TUX64_UNREACHABLE;
}

