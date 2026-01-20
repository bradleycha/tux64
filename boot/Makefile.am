#------------------------------------------------------------------------------#
#                        Copyright (c) Tux64 2025, 2026                   		 #
#                     https://github.com/bradleycha/tux64                      #
#------------------------------------------------------------------------------#
# boot/Makefile.am - GNU Automake script for the bootloader.			 			 #
#------------------------------------------------------------------------------#

noinst_LIBRARIES = \
	libtux64-boot.a

noinst_PROGRAMS = \
	stage0 \
	stage1 \
	stage2

stage1_FONTMAP = \
	$(top_builddir)/src/tux64-boot/stage1/fontmap.bin.c
stage1_STRINGS = \
	$(top_builddir)/src/tux64-boot/stage1/strings.bin.c
stage1_LOGO = \
	$(top_builddir)/src/tux64-boot/stage1/logo.bin.c

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
stage1_BIN_BSS = \
	$(top_builddir)/stage1.bin.bss
stage2_BIN = \
	$(top_builddir)/stage2.bin

stage0_SYM = \
	$(top_builddir)/stage0.sym
stage1_SYM = \
	$(top_builddir)/stage1.sym
stage2_SYM = \
	$(top_builddir)/stage2.sym

BIN_FILES = \
	$(stage0_BIN) \
	$(stage0_BIN_CIC) \
	$(stage1_BIN) \
	$(stage1_BIN_BSS) \
	$(stage2_BIN)

SYM_FILES = \
	$(stage0_SYM) \
	$(stage1_SYM) \
	$(stage2_SYM)

AM_CFLAGS = \
	-I $(top_srcdir)/src \
	-std=c99 \
	-ffreestanding \
	-Wall \
	-Wextra \
	-Wpedantic
AM_LDFLAGS = \
	-nostartfiles

LDADD = \
	-L $(top_srcdir)/src \
	-L $(datadir) \
	$(libdir)/libtux64.a \
	$(top_builddir)/libtux64-boot.a

libtux64_boot_a_SOURCES = \
	src/tux64-boot/tux64-boot.h \
	src/tux64-boot/ipl2.h \
	src/tux64-boot/status.c \
	src/tux64-boot/status.h \
	src/tux64-boot/idle.c \
	src/tux64-boot/idle.h \
	src/tux64-boot/halt.c \
	src/tux64-boot/halt.h \
	src/tux64-boot/sync.c \
	src/tux64-boot/sync.h \
	src/tux64-boot/rsp.c \
	src/tux64-boot/rsp.h \
	src/tux64-boot/pi.c \
	src/tux64-boot/pi.h
stage0_SOURCES = \
	src/tux64-boot/stage0/stage0.s
stage1_SOURCES = \
	src/tux64-boot/stage1/start.c \
	src/tux64-boot/stage1/stage1.c \
	src/tux64-boot/stage1/stage1.h \
	src/tux64-boot/stage1/boot-header.c \
	src/tux64-boot/stage1/boot-header.h \
	src/tux64-boot/stage1/interrupt/interrupt.c \
	src/tux64-boot/stage1/interrupt/interrupt.h \
	src/tux64-boot/stage1/interrupt/entry.s \
	src/tux64-boot/stage1/interrupt/entry.h \
	src/tux64-boot/stage1/preempt.c \
	src/tux64-boot/stage1/preempt.h \
	src/tux64-boot/stage1/format.c \
	src/tux64-boot/stage1/format.h \
	src/tux64-boot/stage1/video.c \
	src/tux64-boot/stage1/video.h \
	src/tux64-boot/stage1/fbcon.c \
	src/tux64-boot/stage1/fbcon.h \
	src/tux64-boot/stage1/logo.c \
	src/tux64-boot/stage1/logo.h \
	src/tux64-boot/stage1/strings.c \
	src/tux64-boot/stage1/strings.h \
	src/tux64-boot/stage1/status.c \
	src/tux64-boot/stage1/status.h
stage2_SOURCES = \
	src/tux64-boot/stage2/stage2.c

EXTRA_DIST = \
	src/tux64-boot/status.ld \
	src/tux64-boot/stage0/stage0.ld \
	src/tux64-boot/stage1/stage1.ld \
	src/tux64-boot/stage1/stack.ld \
	src/tux64-boot/stage1/boot-header.ld \
	src/tux64-boot/stage1/fontmap.rgb888.data \
	src/tux64-boot/stage1/strings.txt \
	src/tux64-boot/stage1/logo.rgba8888.data \
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
	$(BIN_FILES) $(SYM_FILES)

# explicit dependency needed since we're generating the C source code
$(top_srcdir)/src/tux64-boot/stage1/fbcon.c : $(stage1_FONTMAP)
$(top_srcdir)/src/tux64-boot/stage1/strings.c : $(stage1_STRINGS)
$(top_srcdir)/src/tux64-boot/stage1/logo.c : $(stage1_LOGO)

$(stage1_FONTMAP) : $(top_srcdir)/src/tux64-boot/stage1/fontmap.rgb888.data
	$(TUX64_FONTCOMPILER) --input $< --output $@ \
		--name TUX64_BOOT_STAGE1_FONTMAP_BIN

$(stage1_STRINGS) : $(top_srcdir)/src/tux64-boot/stage1/strings.txt
	$(TUX64_TEXTCOMPILER) --input $< --output $@

$(stage1_LOGO) : $(top_srcdir)/src/tux64-boot/stage1/logo.rgba8888.data
	$(TUX64_IMAGECOMPILER) --input $< --output $@ \
		--name-pixels 			TUX64_BOOT_STAGE1_LOGO_IMAGE_PIXELS_COMPRESSED \
		--name-color-table	TUX64_BOOT_STAGE1_LOGO_IMAGE_COLOR_TABLE

# TODO: something is not right here.  if we configure with dependency tracking
# enabled this works fine, but if we disable dependency tracking, this breaks
# in multiple places.  figure out why this happens because this shouldn't
# happen.

$(stage0_BIN) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .rsp_dmem=$@ && chmod -x $@

$(stage0_BIN_CIC) : $(stage0_ELF)
	$(OBJCOPY) $< --dump-section .cic=$@ && chmod -x $@

$(stage1_BIN) : $(stage1_ELF)
	$(OBJCOPY) $< --dump-section .rdram=$@ && chmod -x $@

# TODO: parse the elf files in mkrom, this command is hacky and ridiculous
$(stage1_BIN_BSS) : $(stage1_ELF)
	$(OBJDUMP) -h $< | grep -e "\.rdram\.bss" | grep -e "[0-9a-fA-F]\{8\}" -o | head -n1 > $@

$(stage2_BIN) : $(stage2_ELF)
	$(OBJCOPY) $< --dump-section .rsp_imem=$@ && chmod -x $@

$(stage0_SYM) : $(stage0_ELF)
	$(OBJCOPY) --only-keep-debug $< $@ && chmod -x $@

$(stage1_SYM) : $(stage1_ELF)
	$(OBJCOPY) --only-keep-debug $< $@ && chmod -x $@

$(stage2_SYM) : $(stage2_ELF)
	$(OBJCOPY) --only-keep-debug $< $@ && chmod -x $@

all-local : $(BIN_FILES) $(SYM_FILES)

clean-local :
	rm -f $(BIN_FILES) $(SYM_FILES) $(stage1_FONTMAP) $(stage1_STRINGS) $(stage1_LOGO)

