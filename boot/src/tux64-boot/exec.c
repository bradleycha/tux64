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
#include <tux64/bitwise.h>
#include <tux64/platform/mips/n64/memory-map.h>
#include <tux64/platform/mips/vr4300/cop0.h>
#include "tux64-boot/ipl2.h"
#include "tux64-boot/layout.h"
#include "tux64-boot/load.h"
#include "tux64-boot/halt.h"

__attribute__((section(".kernel_arguments")))
extern struct Tux64BootExecKernelArguments
tux64_boot_exec_kernel_arguments;

static Tux64UInt32
tux64_boot_exec_kernel_arguments_initialize_metadata_word(
   const struct Tux64BootExecKernelMetadata * metadata
) {
   Tux64UInt32 word;

   word = TUX64_LITERAL_UINT32(0u);

   /* we may assume that our enums are always well-defined, i.e. we don't */
   /* ever have invalid enums.  if that's the case, either our bootloader is */
   /* bugged, or the hardware is faulty and should be fixed.  therefore, we */
   /* may use bit tricks to pack our metadata efficiently. */
   word |= (((Tux64UInt32)metadata->console_type)           << TUX64_LITERAL_UINT8(0u));  /* 2 bits */
   word |= (((Tux64UInt32)metadata->ipl2.rom_type)          << TUX64_LITERAL_UINT8(2u));  /* 1 bit  */
   word |= (((Tux64UInt32)metadata->ipl2.reset_type)        << TUX64_LITERAL_UINT8(3u));  /* 1 bit  */
   word |= (((Tux64UInt32)metadata->ipl2.rom_cic_seed)      << TUX64_LITERAL_UINT8(4u));  /* 8 bits */
   word |= (((Tux64UInt32)metadata->ipl2.pif_rom_version)   << TUX64_LITERAL_UINT8(12u)); /* 8 bits */
   return word;
}

void
tux64_boot_exec_kernel_arguments_initialize(
   Tux64UInt32 initramfs_address,
   Tux64UInt32 initramfs_bytes,
   Tux64UInt32 rootfs_cart_address,
   Tux64UInt32 rootfs_bytes,
   Tux64UInt32 command_line_address,
   Tux64UInt32 total_memory,
   Tux64UInt32 rng_seed,
   const struct Tux64BootExecKernelMetadata * metadata
) {
   struct Tux64BootExecKernelArguments * arguments;
   Tux64UInt32 metadata_word;

   arguments = &tux64_boot_exec_kernel_arguments;

   metadata_word = tux64_boot_exec_kernel_arguments_initialize_metadata_word(metadata);

   arguments->initramfs_address     = tux64_endian_convert_uint32(initramfs_address, TUX64_ENDIAN_FORMAT_BIG);
   arguments->initramfs_bytes       = tux64_endian_convert_uint32(initramfs_bytes, TUX64_ENDIAN_FORMAT_BIG);
   arguments->rootfs_cart_address   = tux64_endian_convert_uint32(rootfs_cart_address, TUX64_ENDIAN_FORMAT_BIG);
   arguments->rootfs_bytes          = tux64_endian_convert_uint32(rootfs_bytes, TUX64_ENDIAN_FORMAT_BIG);
   arguments->command_line_address  = tux64_endian_convert_uint32(command_line_address, TUX64_ENDIAN_FORMAT_BIG);
   arguments->total_memory          = tux64_endian_convert_uint32(total_memory, TUX64_ENDIAN_FORMAT_BIG);
   arguments->rng_seed              = tux64_endian_convert_uint32(rng_seed, TUX64_ENDIAN_FORMAT_BIG);
   arguments->metadata              = tux64_endian_convert_uint32(metadata_word, TUX64_ENDIAN_FORMAT_BIG);
   return;
}

void
tux64_boot_exec_kernel(
   const void * entrypoint,
   enum Tux64EndianFormat endian_format
) {
   Tux64UInt32 c0_config;
   Tux64UInt32 fw_arg0_u32;
   Tux64UInt32 fw_arg1_u32;
   Tux64UInt32 fw_arg2_u32;
   Tux64UInt32 fw_arg3_u32;
   unsigned long fw_arg0;
   unsigned long fw_arg1;
   unsigned long fw_arg2;
   unsigned long fw_arg3;

   if (TUX64_BOOT_CONFIG_FOREIGN_ENDIAN_KERNELS) {
      c0_config = tux64_platform_mips_vr4300_cop0_register_read_config();
      if (endian_format != TUX64_ENDIAN_FORMAT_NATIVE) {
         c0_config = tux64_bitwise_flags_flip_uint32(
            c0_config,
            TUX64_PLATFORM_MIPS_VR4300_COP0_CONFIG_BIT_BE
         );
      }
   } else {
      /* assume the caller did the correct checks so ensure we're only */
      /* executing native-endian kernels, as documented. */
      (void)c0_config;
   }

   fw_arg0_u32 = (Tux64UIntPtr)&tux64_boot_exec_kernel_arguments;
   fw_arg1_u32 = TUX64_LITERAL_UINT32(0u);
   fw_arg2_u32 = TUX64_LITERAL_UINT32(0u);
   fw_arg3_u32 = TUX64_LITERAL_UINT32(0u);

   /* since the kernel can be a foreign endianess, we have to do this. */
   fw_arg0 = tux64_endian_convert_uint32(fw_arg0_u32, TUX64_ENDIAN_FORMAT_BIG);
   fw_arg1 = tux64_endian_convert_uint32(fw_arg1_u32, TUX64_ENDIAN_FORMAT_BIG);
   fw_arg2 = tux64_endian_convert_uint32(fw_arg2_u32, TUX64_ENDIAN_FORMAT_BIG);
   fw_arg3 = tux64_endian_convert_uint32(fw_arg3_u32, TUX64_ENDIAN_FORMAT_BIG);

   /* need to write asm explicitly so compiler doesn't bugger up the jump. */
   /* also need .set noreorder so assembler doesn't insert useless nops. */
   __asm__ volatile (
      ".set noreorder\n"
      "jr %0\n"
#if TUX64_BOOT_CONFIG_FOREIGN_ENDIAN_KERNELS
      "mtc0 %1,$%2\n"
#else /* TUX64_BOOT_CONFIG_FOREIGN_ENDIAN_KERNELS */
      "nop\n"
#endif /* TUX64_BOOT_CONFIG_FOREIGN_ENDIAN_KERNELS */
      :: "r"      (entrypoint),
#if TUX64_BOOT_CONFIG_FOREIGN_ENDIAN_KERNELS
         "r"      (c0_config),
         "K"      (TUX64_PLATFORM_MIPS_VR4300_COP0_REGISTER_CAUSE),
#endif /* TUX64_BOOT_CONFIG_FOREIGN_ENDIAN_KERNELS */
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
   (TUX64_BOOT_LAYOUT_STAGE2_STACK_ADDRESS + TUX64_BOOT_LAYOUT_STAGE2_STACK_BYTES)

void
tux64_boot_exec_stage2(
   Tux64BootLoadStatus load_status
) {
   const void * entrypoint;

   entrypoint = (const void *)TUX64_LITERAL_UINTPTR(TUX64_BOOT_LAYOUT_STAGE2_LOAD_ADDRESS);

   /* same issue as above, but now we also have to set the stack pointer. */
   __asm__ volatile (
      ".set noreorder\n"
      "lui $sp,%1\n"
      "jr %0\n"
      "addiu $sp,$sp,%2\n"
      :: "r"      (entrypoint),
         "K"      (TUX64_BOOT_EXEC_STAGE2_STACK_POINTER >> TUX64_LITERAL_UINT8(16u)),
         "K"      (TUX64_BOOT_EXEC_STAGE2_STACK_POINTER & TUX64_LITERAL_UINT32(TUX64_UINT16_MAX)),
         "{s0}"   (load_status)
   );
   TUX64_UNREACHABLE;
}

