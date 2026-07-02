#------------------------------------------------------------------------------#
#                           Copyright (C) Tux64 2026                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# sramdumper/Makefile.am - GNU Automake script for tux64-sramdumper.			 	 #
#------------------------------------------------------------------------------#

bin_PROGRAMS = \
	tux64-sramdumper

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic
LDADD = \
	$(libdir)/libtux64.a

tux64_sramdumper_SOURCES = \
	src/tux64-sramdumper/tux64-sramdumper.c \
	src/tux64-sramdumper/tux64-sramdumper.h \
	src/tux64-sramdumper/arguments.c \
	src/tux64-sramdumper/arguments.h \
	src/tux64-sramdumper/parse.c \
	src/tux64-sramdumper/parse.h \
	src/tux64-sramdumper/format.c \
	src/tux64-sramdumper/format.h

