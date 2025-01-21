#!/bin/sh

# ----------------------------------------------------------------------------
#                          Copyright (c) Tux64 2025
# ----------------------------------------------------------------------------
# buildconf.sh - Global build configuration script template.
# ----------------------------------------------------------------------------

# You can update these environment variables according to your desired setup.
# Please reference the installation guide for more information.

TUX64_BUILD_DIRECTORY=

TUX64_TARGET_HOST=x86_64-pc-linux-gnu
TUX64_TARGET_N64_BOOTLOADER=mips64-elf
TUX64_TARGET_N64_LINUX=mips64-linux-musl
TUX64_MAKEOPTS=${nproc}

TUX64_CFLAGS_HOST="-march=native -O2"
TUX64_CXXFLAGS_HOST="${CFLAGS_HOST}"

TUX64_CFLAGS_N64_COMMON="-march=vr4300 -mfix4300 -Oz -fno-stack-protector -fomit-frame-pointer"
TUX64_CXXFLAGS_N64_COMMON="${CFLAGS_N64}"

TUX64_CFLAGS_N64_BOOTLOADER="${CFLAGS_N64_COMMON}"
TUX64_CXXFLAGS_N64_BOOTLOADER="${CXXFLAGS_N64_COMMON}"

TUX64_CFLAGS_N64_LINUX="${CFLAGS_N64_COMMON}"
TUX64_CXXFLAGS_N64_LINUX="${CFLAGS_N64_COMMON}"

