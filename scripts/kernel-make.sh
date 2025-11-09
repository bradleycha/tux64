#!/bin/sh

# ----------------------------------------------------------------------------
#                          Copyright (c) Tux64 2025
# ----------------------------------------------------------------------------
# This file is licensed under the GPLv3 license.  For more information, see
# LICENSE.
# ----------------------------------------------------------------------------
# scripts/kernel-make.sh - Convenience script for running Linux kernel make
#    commands.
# ----------------------------------------------------------------------------

if [ -z "${TUX64_BUILD_ROOT}" ]; then
   echo "error: \${TUX64_BUILD_ROOT} is not set, make sure to run buildconf.sh in your current shell"
   exit 1
fi

make_target=${1}

make -C ${TUX64_BUILD_ROOT}/sources/linux-* -j${TUX64_MAKEOPTS} \
   O=${TUX64_BUILD_ROOT}/builds/linux \
   ARCH=mips \
   CROSS_COMPILE=${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}- \
   INSTALL_PATH=${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_BOOTLOADER} \
   INSTALL_MOD_PATH=${TUX64_BUILD_ROOT}/rootfs \
   INSTALL_HDR_PATH=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
   KCFLAGS="${TUX64_CFLAGS_N64_KERNEL}" \
   KAFLAGS="${TUX64_ASFLAGS_N64_KERNEL}" \
   KCONFIG_CONFIG=${TUX64_BUILD_ROOT}/builds/linux/kernel-config \
   $make_target

