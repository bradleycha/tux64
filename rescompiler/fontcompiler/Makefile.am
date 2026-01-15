#------------------------------------------------------------------------------#
#                        Copyright (C) Tux64 2025, 2026                        #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# rescompiler/fontcompiler/Makefile.am - GNU Automake script for					 #
# tux64-fontcompiler. 	 	 																	 #
#------------------------------------------------------------------------------#

bin_PROGRAMS = \
	tux64-fontcompiler

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-Wall \
	-Wextra \
	-Wpedantic
LDADD = \
	$(libdir)/libtux64.a

tux64_fontcompiler_SOURCES = \
	src/tux64-fontcompiler/tux64-fontcompiler.c \
	src/tux64-fontcompiler/tux64-fontcompiler.h \
	src/tux64-fontcompiler/arguments.c \
	src/tux64-fontcompiler/arguments.h \
	src/tux64-fontcompiler/generator.c \
	src/tux64-fontcompiler/generator.h

