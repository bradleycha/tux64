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

/* linker defines the memory address, *not* the contents.  this is why we add */
/* a 'const' to prevent accidental assignment, and 'void *' to prevent        */
/* accidental reading.                                                        */
extern void const * _gp;

void
tux64_boot_stage1_start(void) {
   /* we do this so we can read parameters from both IPL2 and stage-0 without */
   /* having to either move registers around or use a custom calling          */
   /* convention.  with unmodified GCC sources, we get spurious pushes for    */
   /* the $sN registers, as well as $ra.  however, with our patched GCC, we   */
   /* add this optimization.  this saves 24 bytes of useless instructions.    */
   /* we also need to explicitly assign to $gp, so we declare it here.        */
   register Tux64UInt32    memory_total      __asm__("$a0");
   register Tux64UInt32    memory_free       __asm__("$a1");
   register Tux64UInt32    running_on_ique   __asm__("$a2");
   register Tux64UInt32    rom_type          __asm__("$s3");
   register Tux64UInt32    video_standard    __asm__("$s4");
   register Tux64UInt32    reset_type        __asm__("$s5");
   register Tux64UInt8     rom_cic_seed      __asm__("$s6");
   register Tux64UInt8     pif_rom_version   __asm__("$s7");
   register void *         gp                __asm__("$gp");

   /* this looks pretty fucked up, so i'll explain.  we have to assign to $gp */
   /* before anything else as the compiler expects it to be initialized for   */
   /* the rest of the code, specifically for small data sections.  _gp has    */
   /* its memory address defined by the linker, not its contents.  that's why */
   /* we load its address, not its contents.  we then have our register       */
   /* variable to tell the compiler to specifically load this into the $gp    */
   /* register.  but if we just try this, the compiler will notice that we    */
   /* aren't technically using gp, thus it can (and will) optimize out the    */
   /* $gp assignment.  this is what the inline asm is for.  if we pass the    */
   /* variable to a volatile inline asm statement, the compiler is forced to  */
   /* actually load $gp.  the asm template is empty because we're not doing   */
   /* the load for $gp, thus we don't need any additional instructions.       */
   /* the reason we don't just write a lui/ori pair ourselves is mainly to    */
   /* give as much power to the compiler as possible to do whatever it thinks */
   /* is optimal (pipelining, reordering, etc.), but also because inline asm  */
   /* is butt-ugly and should be avoided at all costs.                        */
   gp = &_gp;
   __asm__ volatile ("" :: "r" (gp));

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

