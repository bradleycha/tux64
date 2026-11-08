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
      --enable-platform-mips-n64-procfs \
      --enable-platform-mips-vr4300 \
      --enable-log \
      --enable-log-ansi
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

### Chapter 5.2 - Building `tux64-init`

`tux64-init` is our own init process, which is designed to be extremely
minimal and lightweight, while also provided a standard and portable codebase.

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-init
cd ${TUX64_BUILD_ROOT}/builds/tux64-init

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/tux64-*/init/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
      --enable-startup-system-info \
      --enable-procfs \
      --enable-sysfs
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

### Chapter 5.3 - Building `zlib`

`zlib` is a compression library used by various standard tools.

```
mkdir ${TUX64_BUILD_ROOT}/builds/zlib
cd ${TUX64_BUILD_ROOT}/builds/zlib

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
   LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
   ../../sources/zlib-*/configure \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

### Chapter 5.4 - Building `openssl`

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
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
      linux-mips32 \
      zlib-dynamic \
      no-threads \
      no-egd
)

make -j${TUX64_MAKEOPTS} build_libs
make -j${TUX64_MAKEOPTS} install_dev
```

### Chapter 5.5 - Building `gmp`

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
make -j${TUX64_MAKEOPTS} install
```

TODO: this does not work as we get "long long reliability" issues during
configuration.

### Chapter 5.6 - Building `ncurses`

Ncurses is a library which provides terminal control and textual user
interfaces inside a terminal.  It is a dependency for many terminal
applications which provide on-screen menus.

```
mkdir ${TUX64_BUILD_ROOT}/builds/ncurses
cd ${TUX64_BUILD_ROOT}/builds/ncurses

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/ncurses-*/configure \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX} -fno-lto" \
      CXXFLAGS="${TUX64_CXXFLAGS_N64_LINUX} -fno-lto" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX} -fno-lto" \
      --with-shared \
      --with-cxx-shared \
      --without-ada \
      --without-manpages \
      --disable-big-core \
      --disable-lp64 \
      --disable-largefile \
      --without-develop \
      --disable-stripping \
      --enable-overwrite
)
```

Next, we need to manually edit the configuration header to specify the `terminfo`
directory for our installed root.  Without this, programs which use `ncurses`
will fail to start, giving errors about a missing terminals database.
Unfortunately, this seems to be the cleanest way to accomplish this.  Edit
`lib/ncurses_cfg.h` to replace `TERMINFO_DIRS` and `TERMINFO` to look like the
following:

```
#define TERMINFO_DIRS "/usr/share/terminfo"
#define TERMINFO "/usr/share/terminfo"
```

We can now build and install `ncurses`.

```
make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

### Chapter 5.7 - Building `coreutils`

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
make -j${TUX64_MAKEOPTS} install
```

TODO: Build/link against `gmp` once we get it building.

### Chapter 5.8 - Building `util-linux`

`util-linux` provides many standard Linux utilities such as `cal`, `lsblk`,
`more`, `dmesg`, and `hexdump`.

```
mkdir ${TUX64_BUILD_ROOT}/builds/util-linux
cd ${TUX64_BUILD_ROOT}/builds/util-linux

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/util-linux-*/configure \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
      --disable-largefile \
      --disable-assert \
      --disable-nls \
      --disable-asciidoc \
      --disable-poman \
      --disable-tls \
      --disable-widechar \
      --disable-liblastlog2 \
      --disable-pam-lastlog2 \
      --disable-plymouth_support \
      --disable-cramfs \
      --disable-bfs \
      --disable-setpriv \
      --disable-raw \
      --disable-chfn-chsh \
      --disable-login \
      --disable-mount \
      --disable-su \
      --disable-runuser \
      --disable-wall \
      --disable-minix \
      --disable-hwclock-cmos \
      --disable-pylibmount \
      --disable-year2038 \
      --without-ncursesw \
      --with-ncurses \
      --without-btrfs \
      --without-systemd \
      --without-python
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

### Chapter 5.9 - Building `procps`

`procps` provides many command-line utilities for interacting with Linux's
proc filesystem, such as `ps`, `free`, and `top`.

```
mkdir ${TUX64_BUILD_ROOT}/builds/procps
cd ${TUX64_BUILD_ROOT}/builds/procps

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/procps-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
      --disable-largefile \
      --disable-nls \
      --disable-assert \
      --disable-static \
      --enable-shared \
      --disable-numa \
      --without-libintl-prefix
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

TODO: `top` segfaults on start.  This needs to be fixed.

### Chapter 5.10 - Building `bash`

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
```

TODO: this crashes immediately with malloc issues.  I haven't yet debugged why
this is, so we'll need to look into this more.

### Chapter 5.11 - Building `grep`

`grep` is a tool which allows searching for text in files via regular
expressions.

```
mkdir ${TUX64_BUILD_ROOT}/builds/grep
cd ${TUX64_BUILD_ROOT}/builds/grep

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/grep-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
      --disable-threads \
      --disable-assert \
      --disable-nls \
      --disable-perl-regexp \
      --disable-largefile \
      --with-gnu-ld
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

### Chapter 5.12 - Building `sed`

`sed` is a stream editor which is often used in bash scripts for preforming
transformations on text streams.

```
mkdir ${TUX64_BUILD_ROOT}/builds/sed
cd ${TUX64_BUILD_ROOT}/builds/sed

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/sed-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX}" \
      --disable-largefile \
      --disable-year2038 \
      --disable-threads \
      --disable-acl \
      --disable-nls \
      --disable-i18n \
      --without-selinux \
      --with-gnu-ld
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

We will now proceed to [building the kernel](06-building-the-kernel.md).

