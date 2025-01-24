#!/bin/sh

# ----------------------------------------------------------------------------
#                          Copyright (c) Tux64 2025
# ----------------------------------------------------------------------------
# buildconf.sh - Global build configuration script template.
# ----------------------------------------------------------------------------

# You can update these environment variables according to your desired setup.
# Please reference the installation guide for more information.

TUX64_BUILD_ROOT=

TUX64_MAKEOPTS=$(nproc)

TUX64_TARGET_HOST=
TUX64_TARGET_N64_BOOTLOADER=mips64-elf
TUX64_TARGET_N64_LINUX=mips64-linux-musl

TUX64_CFLAGS_HOST="-march=native -O2"
TUX64_CXXFLAGS_HOST="${TUX64_CFLAGS_HOST}"
TUX64_ASFLAGS_HOST=""
TUX64_LDFLAGS_HOST=""

TUX64_CFLAGS_N64_COMMON="-march=vr4300 -mfix4300 -Oz -fno-stack-protector -fomit-frame-pointer -ffunction-sections -fdata-sections"
TUX64_CXXFLAGS_N64_COMMON="${TUX64_CFLAGS_N64_COMMON}"
TUX64_ASFLAGS_N64_COMMON="-march=vr4300 -mtune=vr4300"
TUX64_LDFLAGS_N64_COMMON="-Wl,--gc-sections"

TUX64_CFLAGS_N64_BOOTLOADER="${TUX64_CFLAGS_N64_COMMON}"
TUX64_CXXFLAGS_N64_BOOTLOADER="${TUX64_CXXFLAGS_N64_COMMON}"
TUX64_ASFLAGS_N64_BOOTLOADER="${TUX64_ASFLAGS_N64_COMMON}"
TUX64_LDFLAGS_N64_BOOTLOADER="${TUX64_LDFLAGS_N64_COMMON}"

TUX64_CFLAGS_N64_LINUX="${TUX64_CFLAGS_N64_COMMON}"
TUX64_CXXFLAGS_N64_LINUX="${TUX64_CXXFLAGS_N64_COMMON}"
TUX64_ASFLAGS_N64_LINUX="${TUX64_ASFLAGS_N64_COMMON}"
TUX64_LDFLAGS_N64_LINUX="${TUX64_LDFLAGS_N64_COMMON}"

