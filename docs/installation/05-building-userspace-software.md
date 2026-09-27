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

### Chapter 5.2 - Building `coreutils`

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
      --without-libgmp
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

TODO: Build OpenSSL and GMP so we can use shared code for the above instead of
using duplicate fallback implementations.

### Chapter 5.3 - Building `bash`

TODO: document how to build `bash` so it doesn't immediately crash on startup.

We will now proceed to [building the kernel](06-building-the-kernel.md).

