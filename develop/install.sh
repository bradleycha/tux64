#!/bin/sh

# ----------------------------------------------------------------------------
#                          Copyright (c) Tux64 2025
# ----------------------------------------------------------------------------
# develop/install.sh - Installs development files to the Tux64 build root.
# ----------------------------------------------------------------------------

if [ -z "${TUX64_BUILD_ROOT}" ]; then
   echo "error: \$TUX64_BUILD_ROOT is not set.  please make sure to run buildconf.sh according to the installation guide."
   exit 1
fi

TUX64_INSTALL_DIR_LDSCRIPTS="${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_KERNEL}/lib/tux64"
TUX64_INSTALL_DIR_HEADERS="${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_KERNEL}/include/tux64"

# Create required directories if they don't exist yet
if [ ! -d "${TUX64_INSTALL_DIR_LDSCRIPTS}" ]; then
   mkdir -pv "${TUX64_INSTALL_DIR_LDSCRIPTS}"
fi
if [ ! -d "${TUX64_INSTALL_DIR_HEADERS}" ]; then
   mkdir -pv "${TUX64_INSTALL_DIR_HEADERS}"
fi

# Install linker scripts and headers
cp -rva ./ldscripts/* ${TUX64_INSTALL_DIR_LDSCRIPTS}/
cp -rva ./headers/* ${TUX64_INSTALL_DIR_HEADERS}/

