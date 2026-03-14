/*----------------------------------------------------------------------------*/
/*                          Copyright (C) Tux64 2026                          */
/*                    https://github.com/bradleycha/tux64                     */
/*----------------------------------------------------------------------------*/
/* boot/src/tux64-boot/builtin.h - Header for required GCC builtins.          */
/*----------------------------------------------------------------------------*/

#ifndef _TUX64_BOOT_BUILTIN_H
#define _TUX64_BOOT_BUILTIN_H
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Forces the compiler to correctly link required runtime functions.  This    */
/* compiles to a no-op, and simply adds references to prevent LTO builds from */
/* incorrectly removing "usused" functions before inserting calls to library  */
/* functions.  This should be placed in the startup function of the relevant  */
/* boot stage before any actual code executes, otherwise a false dependency   */
/* will be inserted.                                                          */
/*----------------------------------------------------------------------------*/
void
tux64_boot_builtin_link(void);

/*----------------------------------------------------------------------------*/
#endif /* _TUX64_BOOT_BUILTIN_H */

