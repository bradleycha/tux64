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

typedef void (*Tux64BootExecStartFunction)();

/* like in the stage-1 interrupt handler, gcc will complain if we directly */
/* pointer cast. */
union Tux64BootExecStartPtr {
   Tux64BootExecStartFunction function;
   const void * data;
};

void
tux64_boot_exec_kernel(
   const void * entrypoint,
   const struct Tux64BootExecKernelArguments * arguments
) {
   /* we're manually specifying the registers instead of adding function */
   /* arguments to ensure a stable ABI. */
   union Tux64BootExecStartPtr start; 
   Tux64UInt32 fw_arg0_u32;
   Tux64UInt32 fw_arg1_u32;
   Tux64UInt32 fw_arg2_u32;
   Tux64UInt32 fw_arg3_u32;
   register unsigned long fw_arg0 __asm__("$a0");
   register unsigned long fw_arg1 __asm__("$a1");
   register unsigned long fw_arg2 __asm__("$a2");
   register unsigned long fw_arg3 __asm__("$a3");

   start.data  = entrypoint;

   fw_arg0_u32 = (Tux64UIntPtr)arguments;
   fw_arg1_u32 = TUX64_LITERAL_UINT32(0u);
   fw_arg2_u32 = TUX64_LITERAL_UINT32(0u);
   fw_arg3_u32 = TUX64_LITERAL_UINT32(0u);

   /* theoretically the kernel could be a foreign endianess, so we do this. */
   fw_arg0 = tux64_endian_convert_uint32(fw_arg0_u32, TUX64_ENDIAN_FORMAT_BIG);
   fw_arg1 = tux64_endian_convert_uint32(fw_arg1_u32, TUX64_ENDIAN_FORMAT_BIG);
   fw_arg2 = tux64_endian_convert_uint32(fw_arg2_u32, TUX64_ENDIAN_FORMAT_BIG);
   fw_arg3 = tux64_endian_convert_uint32(fw_arg3_u32, TUX64_ENDIAN_FORMAT_BIG);

   TUX64_EXPLICIT_DEPENDENCY(fw_arg0);
   TUX64_EXPLICIT_DEPENDENCY(fw_arg1);
   TUX64_EXPLICIT_DEPENDENCY(fw_arg2);
   TUX64_EXPLICIT_DEPENDENCY(fw_arg3);
   start.function();
   TUX64_UNREACHABLE;
}

void
tux64_boot_exec_stage2(
   Tux64UInt32 memory_total,
   Tux64BootLoadStatus load_status
) {
   /* same comment as above about the stable ABI.  also, this time, we need */
   /* to manually set the stack pointer. */
   union Tux64BootExecStartPtr   start; 
   register Tux64UInt32          param_memory_total   __asm__("$s0");
   register Tux64BootLoadStatus  param_load_status    __asm__("$s1");
   register Tux64UInt32          stack_pointer        __asm__("$sp");

   start.data = (const void *)TUX64_LITERAL_UINTPTR(TUX64_PLATFORM_MIPS_N64_MEMORY_MAP_ADDRESS_RSP_IMEM);

   param_memory_total   = memory_total;
   param_load_status    = load_status;
   stack_pointer        = TUX64_LITERAL_UINT32(TUX64_BOOT_STAGE2_STACK_ADDRESS + TUX64_BOOT_STAGE2_STACK_BYTES);

   TUX64_EXPLICIT_DEPENDENCY(param_memory_total);
   TUX64_EXPLICIT_DEPENDENCY(param_load_status);
   TUX64_EXPLICIT_DEPENDENCY(stack_pointer);
   start.function();
   TUX64_UNREACHABLE;
}

