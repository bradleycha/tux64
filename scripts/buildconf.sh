#!/bin/sh

# ----------------------------------------------------------------------------
#                          Copyright (c) Tux64 2025
# ----------------------------------------------------------------------------
# This file is licensed under the GPLv3 license.  For more information, see
# LICENSE.
# ----------------------------------------------------------------------------
# buildconf.sh - Global build configuration script template.
# ----------------------------------------------------------------------------

# You can update these environment variables according to your desired setup.
# Please reference the installation guide for more information.

export TUX64_BUILD_ROOT=

export TUX64_MAKEOPTS=$(nproc)

export TUX64_TARGET_HOST=
export TUX64_TARGET_N64_BOOTLOADER=mips64-elf
export TUX64_TARGET_N64_LINUX=mips64-linux-musl

export TUX64_CFLAGS_HOST="-march=native -O2"
export TUX64_CXXFLAGS_HOST="${TUX64_CFLAGS_HOST}"
export TUX64_ASFLAGS_HOST=""
export TUX64_LDFLAGS_HOST=""

export TUX64_CFLAGS_N64_COMMON="-march=vr4300 -mfix4300 -Oz -flto -fno-stack-protector -fomit-frame-pointer -ffunction-sections -fdata-sections"
export TUX64_CXXFLAGS_N64_COMMON="${TUX64_CFLAGS_N64_COMMON}"
export TUX64_ASFLAGS_N64_COMMON="-march=vr4300 -mtune=vr4300"
export TUX64_LDFLAGS_N64_COMMON="-flto -Wl,--gc-sections -s"

export TUX64_CFLAGS_N64_BOOTLOADER="${TUX64_CFLAGS_N64_COMMON}"
export TUX64_CXXFLAGS_N64_BOOTLOADER="${TUX64_CXXFLAGS_N64_COMMON}"
export TUX64_ASFLAGS_N64_BOOTLOADER="${TUX64_ASFLAGS_N64_COMMON}"
export TUX64_LDFLAGS_N64_BOOTLOADER="${TUX64_LDFLAGS_N64_COMMON}"

export TUX64_CFLAGS_N64_LINUX="${TUX64_CFLAGS_N64_COMMON}"
export TUX64_CXXFLAGS_N64_LINUX="${TUX64_CXXFLAGS_N64_COMMON}"
export TUX64_ASFLAGS_N64_LINUX="${TUX64_ASFLAGS_N64_COMMON}"
export TUX64_LDFLAGS_N64_LINUX="${TUX64_LDFLAGS_N64_COMMON}"

