#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# lib/Makefile.am - GNU Automake script for tux64-lib.			 			 		 #
#------------------------------------------------------------------------------#

ACLOCAL_AMFLAGS = -I configdir/m4

lib_LTLIBRARIES = \
	libtux64.la

libtux64_la_CFLAGS = \
	-I$(top_srcdir)/src \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic

libtux64_la_SOURCES = \
	src/tux64/bitwise.c \
	src/tux64/endian.c \
	src/tux64/memory.c \
	src/tux64/log.c \
	src/tux64/arguments.c \
	src/tux64/fs.c \
	src/tux64/string.c \
	src/tux64/math.c \
	src/tux64/checksum.c \
	src/tux64/parse/string-integer.c \
	src/tux64/platform/mips/n64/memory-map.c \
	src/tux64/platform/mips/vr4300/cache.c \
	src/tux64/platform/mips/vr4300/cop0.c

libtux64_includedir = $(pkgincludedir)
libtux64_parse_includedir = $(libtux64_includedir)/parse
libtux64_platform_includedir = $(libtux64_includedir)/platform
libtux64_platform_mips_includedir = $(libtux64_platform_includedir)/mips
libtux64_platform_mips_n64_includedir = $(libtux64_platform_mips_includedir)/n64
libtux64_platform_mips_vr4300_includedir = $(libtux64_platform_mips_includedir)/vr4300

nodist_libtux64_include_HEADERS = \
	$(top_builddir)/configdir/tux64/config.h
libtux64_include_HEADERS = \
	src/tux64/tux64.h \
	src/tux64/types.h \
	src/tux64/bitwise.h \
	src/tux64/endian.h \
	src/tux64/memory.h \
	src/tux64/log.h \
	src/tux64/arguments.h \
	src/tux64/fs.h \
	src/tux64/string.h \
	src/tux64/math.h \
	src/tux64/checksum.h
libtux64_parse_include_HEADERS = \
	src/tux64/parse/string-integer.h
libtux64_platform_mips_n64_include_HEADERS = \
	src/tux64/platform/mips/n64/memory-map.h \
	src/tux64/platform/mips/n64/mmio.h \
	src/tux64/platform/mips/n64/mi.h \
	src/tux64/platform/mips/n64/vi.h \
	src/tux64/platform/mips/n64/ai.h \
	src/tux64/platform/mips/n64/pi.h \
	src/tux64/platform/mips/n64/ri.h \
	src/tux64/platform/mips/n64/si.h \
	src/tux64/platform/mips/n64/rom.h \
	src/tux64/platform/mips/n64/boot.h
libtux64_platform_mips_vr4300_include_HEADERS = \
	src/tux64/platform/mips/vr4300/vr4300.h \
	src/tux64/platform/mips/vr4300/cache.h \
	src/tux64/platform/mips/vr4300/cop0.h

ldscripts_pkgdatadir = $(pkgdatadir)/ldscripts
platform_mips_n64_ldscripts_pkgdatadir = $(ldscripts_pkgdatadir)/platform/mips/n64

dist_platform_mips_n64_ldscripts_pkgdata_DATA = \
	src/tux64/platform/mips/n64/n64.ld \
	src/tux64/platform/mips/n64/memory-map.ld \
	src/tux64/platform/mips/n64/mmio.ld \
	src/tux64/platform/mips/n64/boot.ld

