/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/gp.c - Implementations for $gp register assignment.    */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/gp.h"

/* we have to use a funky type here since we are relying on the linker to     */
/* define its address.  if we try to just use type 'void', gcc will eat it,   */
/* but it's undefined behavior and will generate a warning.  if we make it a  */
/* regular type, for some weird reason it will break in debug builds and just */
/* load $gp with zero.  this type is the only one i've found that works with  */
/* no issues.                                                                 */
extern Tux64UInt8 _gp [];

/* we declare $gp as a global register variable to prevent the compiler from  */
/* optimizing out its assignment without needing crazy inline asm.  this also */
/* gives complete power to the compiler to reorder assignment for whatever it */
/* thinks is optimal.  note that we have to temporarily disable warnings      */
/* because global register variables aren't ISO C compliant (what-evah!).     */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
register void * gp __asm__("$gp");
#pragma GCC diagnostic pop

void
tux64_boot_gp_load(void) {
   /* register variables are implicitly volatile, so it's sufficient to just */
   /* do this.  the compiler will reorder/optimize for us, which is why we */
   /* don't use inline assembly. */
   gp = &_gp;
   return;
}

