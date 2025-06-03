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
libtux64_platform_n64_includedir = $(libtux64_includedir)/platform-n64

libtux64_include_HEADERS = \
	src/tux64.h \
	src/types.h \
	src/config.h
libtux64_platform_n64_include_HEADERS = \
	src/platform-n64/memory-map.h \
	src/platform-n64/mmio.h \
	src/platform-n64/mi.h \
	src/platform-n64/vi.h \
	src/platform-n64/ai.h \
	src/platform-n64/pi.h \
	src/platform-n64/ri.h \
	src/platform-n64/si.h \
	src/platform-n64/rom.h \
	src/platform-n64/boot.h

ldscripts_pkgdatadir = $(pkgdatadir)/ldscripts
platform_n64_ldscripts_pkgdatadir = $(ldscripts_pkgdatadir)/platform-n64

dist_platform_n64_ldscripts_pkgdata_DATA = \
	src/platform-n64/platform-n64.ld \
	src/platform-n64/memory-map.ld \
	src/platform-n64/mmio.ld

