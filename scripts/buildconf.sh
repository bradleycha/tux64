#!/bin/sh

# ----------------------------------------------------------------------------
#                          Copyright (c) Tux64 2025
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
export TUX64_TARGET_N64_KERNEL=mips64-elf
export TUX64_TARGET_N64_USERSPACE=mips64-linux-musl

export TUX64_CFLAGS_HOST="-pipe -march=native -O2 -flto"
export TUX64_CXXFLAGS_HOST="${TUX64_CFLAGS_HOST}"
export TUX64_ASFLAGS_HOST=""
export TUX64_LDFLAGS_HOST="-flto -Wl,--gc-sections -s"

export TUX64_CFLAGS_N64_COMMON="-pipe -march=vr4300 -mfix4300 -mabi=o64 -Oz -flto -fno-stack-protector -fomit-frame-pointer -ffunction-sections -fdata-sections"
export TUX64_CXXFLAGS_N64_COMMON="${TUX64_CFLAGS_N64_COMMON}"
export TUX64_ASFLAGS_N64_COMMON="-march=vr4300 -mtune=vr4300"
export TUX64_LDFLAGS_N64_COMMON="-flto -Wl,--gc-sections -s"

export TUX64_CFLAGS_N64_KERNEL="${TUX64_CFLAGS_N64_COMMON}"
export TUX64_CXXFLAGS_N64_KERNEL="${TUX64_CXXFLAGS_N64_COMMON}"
export TUX64_ASFLAGS_N64_KERNEL="${TUX64_ASFLAGS_N64_COMMON}"
export TUX64_LDFLAGS_N64_KERNEL="${TUX64_LDFLAGS_N64_COMMON}"

export TUX64_CFLAGS_N64_USERSPACE="${TUX64_CFLAGS_N64_COMMON}"
export TUX64_CXXFLAGS_N64_USERSPACE="${TUX64_CXXFLAGS_N64_COMMON}"
export TUX64_ASFLAGS_N64_USERSPACE="${TUX64_ASFLAGS_N64_COMMON}"
export TUX64_LDFLAGS_N64_USERSPACE="${TUX64_LDFLAGS_N64_COMMON}"

