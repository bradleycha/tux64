#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# lib/Makefile.am - GNU Automake script for tux64-lib.			 			 		 #
#------------------------------------------------------------------------------#

ACLOCAL_AMFLAGS = -I configdir/m4

lib_LTLIBRARIES = \
	libtux64.la

libtux64_la_SOURCES =

libtux64_includedir = $(pkgincludedir)
libtux64_platform_n64_includedir = $(libtux64_includedir)/platform_n64

libtux64_include_HEADERS = \
	src/tux64.h \
	src/types.h \
	src/config.h
libtux64_platform_n64_include_HEADERS = \
	src/platform_n64/memory_map.h \
	src/platform_n64/mmio.h \
	src/platform_n64/mi.h \
	src/platform_n64/vi.h \
	src/platform_n64/ai.h \
	src/platform_n64/pi.h \
	src/platform_n64/ri.h \
	src/platform_n64/si.h \
	src/platform_n64/rom.h \
	src/platform_n64/boot.h

# TODO: don't strip the 'platform_n64' part
dist_pkgdata_DATA = \
	src/platform_n64/platform_n64.ld \
	src/platform_n64/memory_map.ld \
	src/platform_n64/mmio.ld

