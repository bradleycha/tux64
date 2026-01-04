#------------------------------------------------------------------------------#
#                        Copyright (c) Tux64 2025, 2026                   		 #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/configure.ac - GNU Autoconf script for the bootloader.                  #
#------------------------------------------------------------------------------#

m4_include([m4/ax_prefix_config_h.m4])

AC_INIT([tux64-boot], 0.1.0+setup, [bradleycha@proton.me])

AC_CONFIG_SRCDIR([src/tux64-boot/tux64-boot.h])
AC_CONFIG_AUX_DIR([configdir])
AC_CONFIG_HEADERS([configdir/config-noprefix.h])

AC_CANONICAL_HOST

if [[ "$host_cpu" != "mips64" ]] || [[ "$host_os" != "elf" ]]; then
   echo configure.ac: error: expected host system type mips64-elf, found $host
   exit 1
fi

AC_CHECK_PROG([TUX64_FONTCOMPILER], [tux64-fontcompiler])
if [[ -z "$TUX64_FONTCOMPILER" ]]; then
   echo configure.ac: error: unable to find tux64-fontcompiler.  please verify the build was configured correctly.
   exit 1
fi

AC_CHECK_PROG([TUX64_TEXTCOMPILER], [tux64-textcompiler])
if [[ -z "$TUX64_TEXTCOMPILER" ]]; then
   echo configure.ac: error: unable to find tux64-textcompiler.  please verify the build was configured correctly.
   exit 1
fi

AC_CHECK_TOOL([OBJCOPY], [objcopy])
if [[ -z "$OBJCOPY" ]]; then
   echo configure.ac: error: unable to find $host_cpu-$host_os-objcopy.  please verify the build was configured correctly.
   exit 1
fi

AC_CHECK_TOOL([OBJDUMP], [objdump])
if [[ -z "$OBJDUMP" ]]; then
   echo configure.ac: error: unable to find $host_cpu-$host_os-objdump.  please verify the build was configured correctly.
   exit 1
fi

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_FILES([Makefile])
AX_PREFIX_CONFIG_H(configdir/tux64-boot/config.h)

AC_PROG_CC
AM_PROG_AS

AC_CHECK_HEADER([tux64/tux64.h])

tux64_boot_config_status=0

AC_ARG_ENABLE([status],
   AS_HELP_STRING([--enable-status], [Write status codes to a fixed memory location during the boot process]),
   [tux64_boot_config_status=1],
   [tux64_boot_config_status=0]
)
AC_ARG_ENABLE([ique],
   AS_HELP_STRING([--enable-ique], [Enable iQue player support]),
   [tux64_boot_config_ique=1],
   [tux64_boot_config_ique=0]
)
AC_ARG_ENABLE([region-pal],
   AS_HELP_STRING([--enable-region-pal], [Enable PAL region support]),
   [tux64_boot_config_region_pal=1],
   [tux64_boot_config_region_pal=0]
)
AC_ARG_ENABLE([region-ntsc],
   AS_HELP_STRING([--enable-region-ntsc], [Enable NTSC region support]),
   [tux64_boot_config_region_ntsc=1],
   [tux64_boot_config_region_ntsc=0]
)
AC_ARG_ENABLE([region-mpal],
   AS_HELP_STRING([--enable-region-mpal], [Enable M-PAL region support]),
   [tux64_boot_config_region_mpal=1],
   [tux64_boot_config_region_mpal=0]
)

AC_DEFINE_UNQUOTED([CONFIG_STATUS],
   [$tux64_boot_config_status],
   [Write status codes to a fixed memory location during the boot process]
)
AC_DEFINE_UNQUOTED([CONFIG_IQUE],
   [$tux64_boot_config_ique],
   [Enable iQue player support]
)
AC_DEFINE_UNQUOTED([CONFIG_REGION_PAL],
   [$tux64_boot_config_region_pal],
   [Enable PAL region support]
)
AC_DEFINE_UNQUOTED([CONFIG_REGION_NTSC],
   [$tux64_boot_config_region_ntsc],
   [Enable NTSC region support]
)
AC_DEFINE_UNQUOTED([CONFIG_REGION_MPAL],
   [$tux64_boot_config_region_mpal],
   [Enable M-PAL region support]
)

AC_OUTPUT

