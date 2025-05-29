#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/configure.ac - GNU Autoconf script for the bootloader.                  #
#------------------------------------------------------------------------------#

AC_INIT([tux64-boot], 0.1.0+setup, [bradleycha@proton.me])

AC_CONFIG_SRCDIR([src/stage0/stage0.s])
AC_CONFIG_AUX_DIR([configdir])
AC_CONFIG_HEADERS([configdir/config.h])

AC_CANONICAL_TARGET

if [[ "$target_cpu" != "mips64" ]]; then
   echo configure.ac: error: expected target cpu mips64, found $target_cpu
   exit 1
fi
if [[ "$target_os" != "elf" ]]; then
   echo configure.ac: error: expected target os elf, found $target_os
   exit 1
fi

AC_CHECK_PROG([OBJCOPY], [objcopy])

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_FILES([Makefile])

AC_PROG_CC
AM_PROG_AS

AC_OUTPUT

