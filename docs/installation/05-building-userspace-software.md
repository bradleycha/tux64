# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 4 - Building Host Software](04-building-host-software.md)
* Next Page: [Chapter 6 - Building The Kernel](06-building-the-kernel.md)

## Chapter 5 - Building Userspace Software

We will now build software which will be installed to the root filesystem for
the Nintendo 64.

### Chapter 5.1 - Building `tux64-lib` For The Nintendo 64 Userspace

Tux64-specific programs which run on the Nintendo 64 require `tux64-lib`, which
must be compiled seperately since the Nintendo 64 userspace is a different
target from our host.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-tux64-lib
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-tux64-lib

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/tux64-*/lib/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
      --enable-platform-cpu-signed-integer-format-twos-complement \
      --enable-platform-mips-n64 \
      --enable-platform-mips-vr4300 \
      --enable-log \
      --enable-log-ansi
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

### Chapter 5.2 - Building `gmp`

`gmp` is the GNU Multi Precision library, and it allows for math on
arbitrarily-large numbers.  It is an optional dependency for `coreutils`, which
we can build as a shared library to reduce memory usage across other packages
which may use `gmp` in the future.

```
mkdir ${TUX64_BUILD_ROOT}/builds/gmp
cd ${TUX64_BUILD_ROOT}/builds/gmp

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/gmp-*/configure \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}"
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

TODO: this does not work as we get "long long reliability" issues during
configuration.

### Chapter 5.3 - Building `openssl`

`openssl` provides cryptography and network communication functions which are
used by many different packages.  Note that if you are compiling a 64-bit
userland, you will need to replace `linux-mips32` with `linux-mips64`.

```
mkdir ${TUX64_BUILD_ROOT}/builds/openssl
cd ${TUX64_BUILD_ROOT}/builds/openssl

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/openssl-*/config \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX} -s" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX} -s" \
      linux-mips32 \
      no-threads \
      no-zlib \
      no-egd
)

make -j${TUX64_MAKEOPTS} build_libs
make -j${TUX64_MAKEOPTS} install_dev
```

### Chapter 5.4 - Building `coreutils`

GNU Coreutils provides basic commands expected on unix-like operating systems,
such as `cat`, `ls`, `whoami`, `uname`, etc.

```
mkdir ${TUX64_BUILD_ROOT}/builds/coreutils
cd ${TUX64_BUILD_ROOT}/builds/coreutils

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/coreutils-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
      --disable-largefile \
      --disable-threads \
      --disable-acl \
      --disable-assert \
      --disable-libsmack \
      --without-libsmack \
      --disable-libcap \
      --disable-nls \
      --without-selinux \
      --without-libgmp \
      --with-openssl=yes
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

TODO: Build/link against `gmp` once we get it building.

### Chapter 5.5 - Building `bash`

`bash` is a standard POSIX-compliant shell which provides the standard
"command-line" across most Linux distributions.

```
mkdir ${TUX64_BUILD_ROOT}/builds/bash
cd ${TUX64_BUILD_ROOT}/builds/bash

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/bash-*/configure \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
      --disable-largefile \
      --disable-nls \
      --disable-threads \
      --enable-year2038 \
      --with-gnu-ld
)

make -j${TUX64_MAKEOPTS}
cp bash ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/bin/bash
${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}-strip ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/bin/bash
```

TODO: this crashes immediately with malloc issues.  I haven't yet debugged why
this is, so we'll need to look into this more.

We will now proceed to [building the kernel](06-building-the-kernel.md).

