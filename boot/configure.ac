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

tux64_boot_err_log() {
   >&2 echo configure.ac: error: $1
}

if [[ "$host_cpu" != "mips64" ]] || [[ "$host_os" != "elf" ]]; then
   $(tux64_boot_err_log "expected host system type mips64-elf, found ${host}")
   exit 1
fi

AC_CHECK_PROG([TUX64_FONTCOMPILER], [tux64-fontcompiler])
if [[ -z "$TUX64_FONTCOMPILER" ]]; then
   $(tux64_boot_err_log "unable to find tux64-fontcompiler.  please verify the build was configured correctly.")
   exit 1
fi

AC_CHECK_PROG([TUX64_TEXTCOMPILER], [tux64-textcompiler])
if [[ -z "$TUX64_TEXTCOMPILER" ]]; then
   $(tux64_boot_err_log "unable to find tux64-textcompiler.  please verify the build was configured correctly.")
   exit 1
fi

AC_CHECK_TOOL([OBJCOPY], [objcopy])
if [[ -z "$OBJCOPY" ]]; then
   $(tux64_boot_err_log "unable to find ${host_cpu}-${host_os}-objcopy.  please verify the build was configured correctly.")
   exit 1
fi

AC_CHECK_TOOL([OBJDUMP], [objdump])
if [[ -z "$OBJDUMP" ]]; then
   $(tux64_boot_err_log "unable to find ${host_cpu}-${host_os}-objdump.  please verify the build was configured correctly.")
   exit 1
fi

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AC_CONFIG_FILES([Makefile])
AX_PREFIX_CONFIG_H(configdir/tux64-boot/config.h)

AC_PROG_CC
AM_PROG_AS
AM_PROG_AR
AC_PROG_RANLIB

AC_CHECK_HEADER([tux64/tux64.h])

AC_ARG_ENABLE([status],
   AS_HELP_STRING([--enable-status], [Write status codes to a fixed memory location during the boot process]),
   [tux64_boot_config_status=1],
   [tux64_boot_config_status=0]
)
AC_ARG_ENABLE([splash],
   AS_HELP_STRING([--enable-splash], [Enable splash text displaying bootloader name and version on startup]),
   [tux64_boot_config_splash=1],
   [tux64_boot_config_splash=0]
)
AC_ARG_ENABLE([logo],
   AS_HELP_STRING([--enable-logo], [Enable drawing the logo image to the background]),
   [tux64_boot_config_logo=1],
   [tux64_boot_config_logo=0]
)
AC_ARG_ENABLE([checksum],
   AS_HELP_STRING([--enable-checksum], [Enable support for checksum verification]),
   [tux64_boot_config_checksum=1],
   [tux64_boot_config_checksum=0]
)
AC_ARG_ENABLE([memory-display],
   AS_HELP_STRING([--enable-memory-display], [Enable support for the display of memory information on startup]),
   [tux64_boot_config_memory_display=1],
   [tux64_boot_config_memory_display=0]
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

if [[ -z "$CONFIG_COLOR_FOREGROUND" ]]; then
   CONFIG_COLOR_FOREGROUND="white"
fi
if [[ -z "$CONFIG_COLOR_BACKGROUND" ]]; then
   CONFIG_COLOR_BACKGROUND="black"
fi

tux64_boot_enumerate_color() {
   case "${1}" in
      "black")
         echo TUX64_BOOT_STAGE1_COLOR_BLACK ;;

      "white")
         echo TUX64_BOOT_STAGE1_COLOR_WHITE ;;

      "gray" | "grey") # show some love for non-americans!
         echo TUX64_BOOT_STAGE1_COLOR_GRAY ;;

      "magenta")
         echo TUX64_BOOT_STAGE1_COLOR_MAGENTA ;;

      "red")
         echo TUX64_BOOT_STAGE1_COLOR_RED ;;

      "orange")
         echo TUX64_BOOT_STAGE1_COLOR_ORANGE ;;

      "yellow")
         echo TUX64_BOOT_STAGE1_COLOR_YELLOW ;;

      "green")
         echo TUX64_BOOT_STAGE1_COLOR_GREEN ;;

      "cyan")
         echo TUX64_BOOT_STAGE1_COLOR_CYAN ;;

      "blue")
         echo TUX64_BOOT_STAGE1_COLOR_BLUE ;;

      "indigo")
         echo TUX64_BOOT_STAGE1_COLOR_INDIGO ;;

      "violet")
         echo TUX64_BOOT_STAGE1_COLOR_VIOLET ;;

      "purple")
         echo TUX64_BOOT_STAGE1_COLOR_PURPLE ;;

      "ruby")
         echo TUX64_BOOT_STAGE1_COLOR_RUBY ;;

      "emerald")
         echo TUX64_BOOT_STAGE1_COLOR_EMERALD ;;

      "sapphire")
         echo TUX64_BOOT_STAGE1_COLOR_SAPPHIRE ;;

      *)
         $(tux64_boot_err_log "invalid $2 color \"${1}\"")
         exit 1 ;;
   esac
}

tux64_boot_config_color_foreground_enum=$(tux64_boot_enumerate_color $CONFIG_COLOR_FOREGROUND "foreground") || exit $?
tux64_boot_config_color_background_enum=$(tux64_boot_enumerate_color $CONFIG_COLOR_BACKGROUND "background") || exit $?

AC_DEFINE_UNQUOTED([CONFIG_STATUS],
   [$tux64_boot_config_status],
   [Write status codes to a fixed memory location during the boot process]
)
AC_DEFINE_UNQUOTED([CONFIG_SPLASH],
   [$tux64_boot_config_splash],
   [Enable splash text displaying bootloader name and version on startup]
)
AC_DEFINE_UNQUOTED([CONFIG_LOGO],
   [$tux64_boot_config_logo],
   [Enable drawing the logo image to the background]
)
AC_DEFINE_UNQUOTED([CONFIG_CHECKSUM],
   [$tux64_boot_config_checksum],
   [Enable support for checksum verification]
)
AC_DEFINE_UNQUOTED([CONFIG_MEMORY_DISPLAY],
   [$tux64_boot_config_memory_display],
   [Enable support for the display of memory information on startup]
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
AC_DEFINE_UNQUOTED([CONFIG_COLOR_FOREGROUND],
   [$tux64_boot_config_color_foreground_enum],
   [The foreground color for the console]
)
AC_DEFINE_UNQUOTED([CONFIG_COLOR_BACKGROUND],
   [$tux64_boot_config_color_background_enum],
   [The foreground color for the console]
)

AC_OUTPUT

