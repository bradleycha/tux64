#------------------------------------------------------------------------------#
#                           Copyright (c) Tux64 2025                           #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/Makefile.am - GNU Automake script for the bootloader.			 			 #
#------------------------------------------------------------------------------#

noinst_PROGRAMS = \
	stage0 \
	stage1 \
	stage2

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

stage0_DBG = \
	$(top_builddir)/stage0.dbg
stage1_DBG = \
	$(top_builddir)/stage1.dbg
stage2_DBG = \
	$(top_builddir)/stage2.dbg

BIN_FILES = \
	$(stage0_BIN) \
	$(stage0_BIN_CIC) \
	$(stage1_BIN) \
	$(stage2_BIN)

DBG_FILES = \
	$(stage0_DBG) \
	$(stage1_DBG) \
	$(stage2_DBG)

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-ffreestanding \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Werror
AM_LDFLAGS = \
	-nostartfiles

LDADD = \
	-L $(top_srcdir)/src \
	-L $(datadir) \
	$(libdir)/libtux64.a

stage0_SOURCES = \
	src/tux64-boot/stage0/stage0.s
stage1_SOURCES = \
	src/tux64-boot/stage1/stage1.c
stage2_SOURCES = \
	src/tux64-boot/stage2/stage2.c

noinst_HEADERS = \
	src/tux64-boot/tux64-boot.h

EXTRA_DIST = \
	src/tux64-boot/stage0/stage0.ld \
	src/tux64-boot/stage1/stage1.ld \
	src/tux64-boot/stage2/stage2.ld

stage0_LDFLAGS = \
	$(AM_LDFLAGS) \
	-T $(top_srcdir)/src/tux64-boot/stage0/stage0.ld
stage1_LDFLAGS = \
	$(AM_LDFLAGS) \
	-T $(top_srcdir)/src/tux64-boot/stage1/stage1.ld
stage2_LDFLAGS = \
	$(AM_LDFLAGS) \
	-T $(top_srcdir)/src/tux64-boot/stage2/stage2.ld

pkgdata_DATA = \
	$(BIN_FILES) $(DBG_FILES)

$(stage0_BIN) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .rsp_dmem=$@ && chmod -x $@

$(stage0_BIN_CIC) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .cic=$@ && chmod -x $@

$(stage1_BIN) : $(stage1_ELF)
	$(OBJCOPY) $< --dump-section .rdram=$@ && chmod -x $@

$(stage2_BIN) : $(stage2_ELF)
	$(OBJCOPY) $< --dump-section .rsp_imem=$@ && chmod -x $@

$(stage0_DBG) : $(stage0_ELF)
	$(OBJCOPY) --only-keep-debug $< $@ && chmod -x $@

$(stage1_DBG) : $(stage1_ELF)
	$(OBJCOPY) --only-keep-debug $< $@ && chmod -x $@

$(stage2_DBG) : $(stage2_ELF)
	$(OBJCOPY) --only-keep-debug $< $@ && chmod -x $@

all-local : $(BIN_FILES) $(DBG_FILES)

clean-local :
	rm -f $(BIN_FILES) $(DBG_FILES)

