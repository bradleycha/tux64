#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# textcompiler/Makefile.am - GNU Automake script for tux64-textcompiler. 	 	 #
#------------------------------------------------------------------------------#

bin_PROGRAMS = \
	tux64-textcompiler

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic
LDADD = \
	$(libdir)/libtux64.a

tux64_textcompiler_SOURCES = \
	src/tux64-textcompiler/tux64-textcompiler.c \
	src/tux64-textcompiler/tux64-textcompiler.h \
	src/tux64-textcompiler/arguments.c \
	src/tux64-textcompiler/arguments.h \
	src/tux64-textcompiler/lexer.c \
	src/tux64-textcompiler/lexer.h \
	src/tux64-textcompiler/formatter.c \
	src/tux64-textcompiler/formatter.h

