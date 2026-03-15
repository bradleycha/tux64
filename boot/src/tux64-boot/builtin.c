/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/builtin.c - Implementations for required GCC builtins. */
/*----------------------------------------------------------------------------*/

#include "tux64-boot/tux64-boot.h"
#include "tux64-boot/builtin.h"

#include <tux64/memory.h>
#include "tux64-boot/halt.h"

#include <stddef.h>

/*----------------------------------------------------------------------------*/
/* okay, this is complete and utter fucking bullshit...                       */
/* we're telling the fucker to not use library functions (-ffreestanding)     */
/* since we don't have libc available.  however, the following functions can  */
/* still be implicitly compiled in, despite us telling this cock sucker to    */
/* specifically not do this:                                                  */
/*                                                                            */
/*    memcpy()                                                                */
/*    memmove()                                                               */
/*    memset()                                                                */
/*    memcmp()                                                                */
/*    abort()                                                                 */
/*                                                                            */
/* don't believe me?  maybe you'll believe the gcc developers.  go to the     */
/* following part of the gcc manual and feast your eyes upon this:            */
/*                                                                            */
/*             https://gcc.gnu.org/onlinedocs/gcc/Standards.html              */
/*                                                                            */
/*  "Most of the compiler support routines used by GCC are present in         */
/*   libgcc, but there are a few exceptions. GCC requires the freestanding    */
/*   environment provide memcpy, memmove, memset and memcmp. Contrary to      */
/*   the standards covering memcpy GCC expects the case of an exact overlap   */
/*   of source and destination to work and not invoke undefined behavior.     */
/*   Finally, if __builtin_trap is used, and the target does not implement    */
/*   the trap pattern, then GCC emits a call to abort."                       */
/*                                                                            */
/* yeah...so instead of fixing their fucking compiler, or at least making an  */
/* option to really, really, ***REALLY*** disable these, they instead         */
/* document the broken shit as intentional, like wtf???                       */
/*                                                                            */
/* yes, i understand that having the compiler emit byte copy loops everywhere */
/* is insanely shit in its own right, but we're *specifially telling gcc* we  */
/* don't have a standard library, and thus don't have these functions...but   */
/* hey, guess now the compiler can emit calls to memcpy() for us! woo-hoo!    */
/*                                                                            */
/* the world is a cruel place.  in a world filled with rust crusaders, vibe   */
/* "coding" grifters, and soydev javascript pussies, i thought i could trust  */
/* gcc.  i thought i could trust you.  but no.  instead, you backstab me when */
/* i least expect it.  i try upgrading packages and recompiling my build      */
/* root.  what once worked no longer does.  you were doing as told before,    */
/* but now that you've gained my trust, you wait until i push commits         */
/* publically before destroying my code, leaving ruins of bizzare build       */
/* failures and mysteriously crashing programs.  i cannot trust anyone        */
/* anymore.  not even my own C compiler.                                      */
/*----------------------------------------------------------------------------*/

/* guess what? the shit deepens! because we're implementing library           */
/* functions, the compiler might recognize the pattern and replace it with a  */
/* call to the library function, leading to infinite recursion!  we need this */
/* optimize(...) garbage to get the compiler to bugger off.                   */
/*                                                                            */
/* even better! we need to declare these builtins in their own special        */
/* sections, which can allow --gc-sections to remove them if they are unused. */
/* without this, they will always be included with the trick we are force to  */
/* use below to prevent LTO from removing them.  this also means we have to   */
/* change the default LDFLAGS to prevent a regression.                        */
/*                                                                            */
/* what an absolute pile of fuck.                                             */
#define TUX64_BOOT_BUILTIN_IMPLEMENTATION(identifier) \
   optimize("no-tree-loop-distribute-patterns"), \
   section(".text.tux64_boot.builtin." identifier)

__attribute__((TUX64_BOOT_BUILTIN_IMPLEMENTATION("memcpy")))
void *
memcpy(
   void * restrict dest,
   const void * restrict src,
   size_t n
) {
   tux64_memory_copy(dest, src, (Tux64UInt32)n);
   return dest;
}

__attribute__((TUX64_BOOT_BUILTIN_IMPLEMENTATION("memmove")))
void *
memmove(
   void * dest,
   const void * src,
   size_t n
) {
   tux64_memory_move(dest, src, (Tux64UInt32)n);
   return dest;
}

__attribute__((TUX64_BOOT_BUILTIN_IMPLEMENTATION("memset")))
void *
memset(
   void * s,
   int c,
   size_t n
) {
   Tux64UInt8 byte;

   byte = (Tux64UInt8)c;

   tux64_memory_fill(
      s,
      &byte,
      (Tux64UInt32)n,
      TUX64_LITERAL_UINT32(sizeof(byte))
   );

   return s;
}

__attribute__((TUX64_BOOT_BUILTIN_IMPLEMENTATION("memcmp")))
int
memcmp(
   const void * s1,
   const void * s2,
   size_t n
) {
   const Tux64UInt8 * iter_s1;
   const Tux64UInt8 * iter_s2;
   Tux64UInt8 byte_s1;
   Tux64UInt8 byte_s2;
   int difference;

   /* this function spec. is so fucked up and incompatible with everything, */
   /* we have no choice but to go fully hand-made here. */

   iter_s1 = (const Tux64UInt8 *)s1;
   iter_s2 = (const Tux64UInt8 *)s2;

   difference = 0;

   while (n != 0u) {
      byte_s1 = *iter_s1++;
      byte_s2 = *iter_s2++;
      n--;

      difference += ((int)byte_s1 - (int)byte_s2);
   }

   return difference;
}

__attribute__((TUX64_BOOT_BUILTIN_IMPLEMENTATION("abort"), noreturn))
void
abort(void) {
   tux64_boot_halt();
   TUX64_UNREACHABLE;
}

/* good lord, don't even get me started on the multi-day LTO debugging hell */
/* required to arrive at this horrendous macro garbage.  trust me, i tried */
/* to get anything but this, but this shit was the only thing that worked. */
#define TUX64_BOOT_BUILTIN_FORCE_LINKAGE(identifier) \
   __asm__ volatile ("" :: "X" (identifier))

void
tux64_boot_builtin_link(void) {
   TUX64_BOOT_BUILTIN_FORCE_LINKAGE(memcpy);
   TUX64_BOOT_BUILTIN_FORCE_LINKAGE(memmove);
   TUX64_BOOT_BUILTIN_FORCE_LINKAGE(memset);
   TUX64_BOOT_BUILTIN_FORCE_LINKAGE(memcmp);
   TUX64_BOOT_BUILTIN_FORCE_LINKAGE(abort);
   return;
}

