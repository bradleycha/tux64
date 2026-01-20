#------------------------------------------------------------------------------#
#                           Copyright (C) Tux64 2026                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# rescompiler/imagecompiler/configure.ac - GNU Autoconf script for             #
#    tux64-imagecompiler.                                                      #
#------------------------------------------------------------------------------#

m4_include([m4/ax_prefix_config_h.m4])

AC_INIT([tux64-imagecompiler], 0.1.0+setup, [bradleycha@proton.me])

AC_CONFIG_SRCDIR([src/tux64-imagecompiler/tux64-imagecompiler.h])
AC_CONFIG_AUX_DIR([configdir])
AC_CONFIG_HEADERS([configdir/config-noprefix.h])

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_FILES([Makefile])

AC_PROG_CC

AC_CHECK_HEADER([tux64/tux64.h])

AX_PREFIX_CONFIG_H(configdir/tux64-imagecompiler/config.h)

AC_OUTPUT

