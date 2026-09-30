#!/bin/sh

# ----------------------------------------------------------------------------
#                        Copyright (c) Tux64 2025, 2026                   		
# ----------------------------------------------------------------------------
# This file is licensed under the GPLv3 license.  For more information, see
# LICENSE.
# ----------------------------------------------------------------------------
# scripts/buildconf.sh - Global build configuration script template.
# ----------------------------------------------------------------------------

# You can update these environment variables according to your desired setup.
# Please reference the installation guide for more information.

export TUX64_BUILD_ROOT=

export TUX64_MAKEOPTS=$(nproc)

export TUX64_TARGET_HOST=
export TUX64_TARGET_N64_BOOTLOADER=mips64-elf
export TUX64_TARGET_N64_LINUX=mips-linux-musl

export TUX64_CFLAGS_COMMON="-pipe -flto -ffunction-sections -fdata-sections -s"
export TUX64_CXXFLAGS_COMMON=""
export TUX64_ASFLAGS_COMMON=""
export TUX64_LDFLAGS_COMMON="-Wl,--gc-sections"

export TUX64_CFLAGS_HOST="${TUX64_CFLAGS_COMMON} -march=native -O2"
export TUX64_CXXFLAGS_HOST="${TUX64_CFLAGS_HOST} ${TUX64_CXXFLAGS_COMMON}"
export TUX64_ASFLAGS_HOST="${TUX64_ASFLAGS_COMMON}"
export TUX64_LDFLAGS_HOST="${TUX64_CFLAGS_HOST} ${TUX64_LDFLAGS_COMMON}"

export TUX64_CFLAGS_N64_COMMON="${TUX64_CFLAGS_COMMON} -march=vr4300 -mfix4300 -Oz -fno-stack-protector -mno-check-zero-division"
export TUX64_CXXFLAGS_N64_COMMON="${TUX64_CFLAGS_N64_COMMON} ${TUX64_CXXFLAGS_COMMON}"
export TUX64_ASFLAGS_N64_COMMON="${TUX64_ASFLAGS_COMMON} -march=vr4300 -mtune=vr4300"
export TUX64_LDFLAGS_N64_COMMON="${TUX64_CFLAGS_N64_COMMON} ${TUX64_LDFLAGS_COMMON}"

export TUX64_CFLAGS_N64_BOOTLOADER="${TUX64_CFLAGS_N64_COMMON} -mabi=o64 -G65536 -mexplicit-relocs=none"
export TUX64_ASFLAGS_N64_BOOTLOADER="${TUX64_ASFLAGS_N64_COMMON}"
export TUX64_LDFLAGS_N64_BOOTLOADER="${TUX64_LDFLAGS_N64_COMMON}"

export TUX64_CFLAGS_N64_KERNEL="${TUX64_CFLAGS_N64_COMMON} -fno-lto"
export TUX64_ASFLAGS_N64_KERNEL="${TUX64_ASFLAGS_N64_COMMON}"

export TUX64_CFLAGS_N64_LINUX="${TUX64_CFLAGS_N64_COMMON} -mabi=32"
export TUX64_CXXFLAGS_N64_LINUX="${TUX64_CFLAGS_N64_LINUX}"
export TUX64_ASFLAGS_N64_LINUX="${TUX64_ASFLAGS_N64_COMMON}"
export TUX64_LDFLAGS_N64_LINUX="${TUX64_LDFLAGS_N64_COMMON}"

