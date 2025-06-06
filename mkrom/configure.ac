#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# mkrom/configure.ac - GNU Autoconf script for tux64-mkrom.                    #
#------------------------------------------------------------------------------#

m4_include([m4/ax_prefix_config_h.m4])

AC_INIT([tux64-mkrom], 0.1.+setup, [bradleycha@proton.me])

AC_CONFIG_SRCDIR([src/tux64-mkrom/tux64-mkrom.h])
AC_CONFIG_AUX_DIR([configdir])
AC_CONFIG_HEADERS([configdir/config-noprefix.h])

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_FILES([Makefile])

AC_PROG_CC

AC_CHECK_HEADER([tux64/tux64.h])
AC_CHECK_HEADER([argp.h])
AX_PREFIX_CONFIG_H(configdir/tux64-mkrom/config.h)

AC_OUTPUT

