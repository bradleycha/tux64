#------------------------------------------------------------------------------#
#                           Copyright (C) Tux64 2026                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# sramdumper/Makefile.am - GNU Automake script for tux64-sramdumper.			 	 #
#------------------------------------------------------------------------------#

ACLOCAL_AMFLAGS = -I configdir/m4

bin_PROGRAMS = \
	tux64-sramdumper

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic
LDADD = \
	$(libdir)/libtux64-core.la \
	$(libdir)/libtux64.la

tux64_sramdumper_SOURCES = \
	src/tux64-sramdumper/tux64-sramdumper.c \
	src/tux64-sramdumper/tux64-sramdumper.h \
	src/tux64-sramdumper/arguments.c \
	src/tux64-sramdumper/arguments.h \
	src/tux64-sramdumper/parse.c \
	src/tux64-sramdumper/parse.h \
	src/tux64-sramdumper/format.c \
	src/tux64-sramdumper/format.h

