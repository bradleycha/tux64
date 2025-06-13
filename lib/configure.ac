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

AC_CHECK_SIZEOF([char])
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([float])
AC_CHECK_SIZEOF([double])
AC_CHECK_SIZEOF([void *])

AC_CHECK_HEADERS([stdint.h stdarg.h stdlib.h string.h stdio.h errno.h])

tux64_config_platform_cpu_signed_integer_format_sign_magnitude=0
tux64_config_platform_cpu_signed_integer_format_ones_complement=0
tux64_config_platform_cpu_signed_integer_format_twos_complement=1
tux64_config_platform_n64=0
tux64_config_log=0
tux64_config_log_origin=0
tux64_config_log_ansi=0

AC_ARG_ENABLE([platform-cpu-signed-integer-format-sign-magnitude],
   AS_HELP_STRING([--enable-platform-cpu-signed-integer-format-sign-magnitude], [Assume the platform CPU's signed integer format is sign-magnitude]),
   [tux64_config_platform_cpu_signed_integer_format_sign_magnitude=1],
   [tux64_config_platform_cpu_signed_integer_format_sign_magnitude=0]
)
AC_ARG_ENABLE([platform-cpu-signed-integer-format-ones-complement],
   AS_HELP_STRING([--enable-platform-cpu-signed-integer-format-ones-complement], [Assume the platform CPU's signed integer format is one's complement]),
   [tux64_config_platform_cpu_signed_integer_format_ones_complement=1],
   [tux64_config_platform_cpu_signed_integer_format_ones_complement=0]
)
AC_ARG_ENABLE([platform-cpu-signed-integer-format-twos-complement],
   AS_HELP_STRING([--enable-platform-cpu-signed-integer-format-twos-complement], [Assume the platform CPU's signed integer format is two's complement]),
   [tux64_config_platform_cpu_signed_integer_format_twos_complement=1],
   [tux64_config_platform_cpu_signed_integer_format_twos_complement=0]
)
AC_ARG_ENABLE([platform-n64],
   AS_HELP_STRING([--enable-platform-n64], [Enable N64-specific features]),
   [tux64_config_platform_n64=1],
   [tux64_config_platform_n64=0]
)
AC_ARG_ENABLE([log],
   AS_HELP_STRING([--enable-log], [Enable logging functions, requires stdio.h and stdarg.h]),
   [tux64_config_log=1],
   [tux64_config_log=0]
)
AC_ARG_ENABLE([log-origin],
   AS_HELP_STRING([--enable-log-origin], [Display the location in the source file for each log message]),
   [tux64_config_log_origin=1],
   [tux64_config_log_origin=0]
)
AC_ARG_ENABLE([log-ansi],
   AS_HELP_STRING([--enable-log-ansi], [Format log messages using ANSI escape sequences to enable colored text]),
   [tux64_config_log_ansi=1],
   [tux64_config_log_ansi=0]
)

AC_DEFINE_UNQUOTED([CONFIG_PLATFORM_CPU_SIGNED_INTEGER_FORMAT_SIGN_MAGNITUDE],
   [$tux64_config_platform_cpu_signed_integer_format_sign_magnitude],
   [Assume the platform CPU's signed integer format is sign-magnitude]
)
AC_DEFINE_UNQUOTED([CONFIG_PLATFORM_CPU_SIGNED_INTEGER_FORMAT_ONES_COMPLEMENT],
   [$tux64_config_platform_cpu_signed_integer_format_ones_complement],
   [Assume the platform CPU's signed integer format is one's complement]
)
AC_DEFINE_UNQUOTED([CONFIG_PLATFORM_CPU_SIGNED_INTEGER_FORMAT_TWOS_COMPLEMENT],
   [$tux64_config_platform_cpu_signed_integer_format_twos_complement],
   [Assume the platform CPU's signed integer format is two's complement]
)
AC_DEFINE_UNQUOTED([CONFIG_PLATFORM_N64],
   [$tux64_config_platform_n64],
   [Enable N64-specific features]
)
AC_DEFINE_UNQUOTED([CONFIG_LOG],
   [$tux64_config_log],
   [Enable logging functions, requires stdio.h and stdarg.h]
)
AC_DEFINE_UNQUOTED([CONFIG_LOG_ORIGIN],
   [$tux64_config_log_origin],
   [Display the location in the source file for each log message]
)
AC_DEFINE_UNQUOTED([CONFIG_LOG_ANSI],
   [$tux64_config_log_ansi],
   [Format log messages using ANSI escape sequences to enable colored text]
)

AC_OUTPUT

