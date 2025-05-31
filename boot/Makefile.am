#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/Makefile.am - GNU Automake script for the bootloader.			 			 #
#------------------------------------------------------------------------------#

bin_PROGRAMS = \
	stage0 \
	stage1 \
	stage2

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-ffreestanding \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Werror
LDADD = \
	-L $(top_srcdir)/src

SHARED_SOURCES = \
	src/tux64_boot.h

stage0_SOURCES = \
	$(SHARED_SOURCES) \
	src/stage0/stage0.ld \
	src/stage0/stage0.s
stage1_SOURCES = \
	$(SHARED_SOURCES) \
	src/stage1/stage1.ld \
	src/stage1/stage1.c
stage2_SOURCES = \
	$(SHARED_SOURCES) \
	src/stage2/stage2.ld \
	src/stage2/stage2.c \
	src/stage2/payload.h \
	src/stage2/payload.ld

stage0_LDFLAGS = \
	-T $(top_srcdir)/src/stage0/stage0.ld
stage1_LDFLAGS = \
	-T $(top_srcdir)/src/stage1/stage1.ld
stage2_LDFLAGS = \
	-T $(top_srcdir)/src/stage2/stage2.ld

stage0_ELF = \
	$(top_builddir)/stage0
stage1_ELF = \
	$(top_builddir)/stage1
stage2_ELF = \
	$(top_builddir)/stage2

stage0_BIN = \
	$(top_builddir)/stage0.bin
stage0_BIN_CIC = \
	$(top_builddir)/stage0.bin.cic
stage1_BIN = \
	$(top_builddir)/stage1.bin
stage2_BIN = \
	$(top_builddir)/stage2.bin

BIN_FILES = \
	$(stage0_BIN) \
	$(stage0_BIN_CIC) \
	$(stage1_BIN) \
	$(stage2_BIN)

$(stage0_BIN) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .rsp_dmem=$@ && chmod -x $@

$(stage0_BIN_CIC) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .cic=$@ && chmod -x $@

$(stage1_BIN) : $(stage1_ELF)
	$(OBJCOPY) $< --dump-section .rdram=$@ && chmod -x $@

$(stage2_BIN) : $(stage2_ELF)
	$(OBJCOPY) $< --dump-section .rsp_imem=$@ && chmod -x $@

all-local : $(BIN_FILES)

clean-local :
	rm -f $(BIN_FILES)

# TODO: generate stage1.bin.bss, currently we have no idea how big .rdram.bss
# is since we strip all section information in stage1.bin

# TODO: install .bin files instead of ELF files

