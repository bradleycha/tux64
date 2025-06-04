#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# mkrom/configure.ac - GNU Autoconf script for tux64-mkrom.                    #
#------------------------------------------------------------------------------#

AC_INIT([tux64-mkrom], 0.1.+setup, [bradleycha@proton.me])

AC_CONFIG_SRCDIR([src/tux64-mkrom/tux64-mkrom.h])
AC_CONFIG_AUX_DIR([configdir])
AC_CONFIG_HEADERS([configdir/config.h])

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_FILES([Makefile])

AC_PROG_CC

AC_CHECK_HEADER([tux64/tux64.h])

AC_OUTPUT

