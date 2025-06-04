#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# lib/configure.ac - GNU Autoconf script for tux64-lib.                        #
#------------------------------------------------------------------------------#

m4_include([m4/ax_prefix_config_h.m4])

AC_INIT([tux64], 0.1.0+setup, [bradleycha@proton.me])

AC_CONFIG_SRCDIR([src/tux64/tux64.h])
AC_CONFIG_AUX_DIR([configdir])
AC_CONFIG_HEADERS([configdir/config-noprefix.h])

AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIRS([configdir/m4])
AX_PREFIX_CONFIG_H(configdir/tux64/config.h)

AC_PROG_CC
AM_PROG_AR
LT_INIT

tux64_config_platform_n64=0

AC_ARG_ENABLE([platform-n64],
   AS_HELP_STRING([--enable-platform-n64], [Enable N64-specific features]),
   [tux64_config_platform_n64=1],
   [tux64_config_platform_n64=0]
)

AC_DEFINE_UNQUOTED([CONFIG_PLATFORM_N64],
   [$tux64_config_platform_n64],
   [Enable N64-specific features]
)

AC_OUTPUT

