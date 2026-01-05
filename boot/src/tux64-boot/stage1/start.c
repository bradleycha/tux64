/*----------------------------------------------------------------------------*/
/*                       Copyright (C) Tux64 2025, 2026                       */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/stage1/start.c - Stage-1 bootloader startup code.      */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/stage1/stage1.h"
#include "tux64-boot/ipl2.h"

void
tux64_boot_stage1_start(void)
__attribute__((noreturn, section(".start"), externally_visible));

void
tux64_boot_stage1_start(void) {
   /* we do this so we can read parameters from both IPL2 and stage-0 without */
   /* having to either move registers around or use a custom calling */
   /* convention.  unfortunately, we get spurious pushes of the $sN registers */
   /* because for some reason the compiler is still preserving non-volatiles */
   /* despite the function being noreturn.  the only solution to this is to */
   /* define global register variables, which restrict the registers */
   /* throughout the entire program when LTO is enabled and also causes */
   /* strange build failures.  it's not worth saving 20 bytes which execute */
   /* once at startup.  if we really want to fix this properly, we'll need to */
   /* write compiler patches. */
   register Tux64UInt32 memory_total      __asm__("$a0");
   register Tux64UInt32 memory_free       __asm__("$a1");
   register Tux64UInt32 running_on_ique   __asm__("$a2");
   register Tux64UInt32 rom_type          __asm__("$s3");
   register Tux64UInt32 video_standard    __asm__("$s4");
   register Tux64UInt32 reset_type        __asm__("$s5");
   register Tux64UInt8  rom_cic_seed      __asm__("$s6");
   register Tux64UInt8  pif_rom_version   __asm__("$s7");

   /* we have to initialize $gp for use with small data sections.  "_gp" is */
   /* defined by the linker. */
   __asm__ volatile (
      "lui $gp,%hi(_gp)\n"
      "addiu $gp,$gp,%lo(_gp)\n"
   );

   tux64_boot_stage1_main(
      (enum Tux64BootIpl2RomType)rom_type,
      (enum Tux64BootIpl2VideoStandard)video_standard,
      (enum Tux64BootIpl2ResetType)reset_type,
      rom_cic_seed,
      pif_rom_version,
      memory_total,
      memory_free,
      running_on_ique == TUX64_LITERAL_UINT32(0u)
         ? TUX64_BOOLEAN_FALSE
         : TUX64_BOOLEAN_TRUE
   );

   TUX64_UNREACHABLE;
}

