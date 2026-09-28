#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2026                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# init/configure.ac - GNU Autoconf script for tux64-init.                      #
#------------------------------------------------------------------------------#

m4_include([m4/ax_prefix_config_h.m4])

AC_INIT([tux64-init], 0.1.0+setup, [bradleycha@proton.me])

AC_CONFIG_SRCDIR([src/tux64-init/tux64-init.h])
AC_CONFIG_AUX_DIR([configdir])
AC_CONFIG_HEADERS([configdir/config-noprefix.h])

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIRS([configdir/m4])
AX_PREFIX_CONFIG_H(configdir/tux64-init/config.h)

AC_PROG_CC
LT_INIT

AC_CHECK_HEADER([tux64/tux64.h])

AC_ARG_ENABLE([startup-system-info],
   AS_HELP_STRING([--enable-startup-system-info], [Enable printing system information on startup.]),
   [tux64_init_startup_system_info=1],
   [tux64_init_startup_system_info=0]
)

AC_DEFINE_UNQUOTED([CONFIG_STARTUP_SYSTEM_INFO],
   [$tux64_init_startup_system_info],
   [Enable printing system information on startup.]
)

AC_OUTPUT

