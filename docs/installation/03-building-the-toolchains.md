# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 2 - Obtaining Sources](02-obtaining-sources.md)
* Next Page: [Chapter 4 - Building Userspace Software](04-building-userspace-software.md)

## Chapter 3 - Building The Toolchains

In order to build code for the Nintendo 64, we need to create a toolchain which
can build such code.  In fact, we need multiple toolchains, as we are building
both binaries to run under Linux and flat code to run without an operating
system.  We also want to use the 3-stage bootstrapping process to get a
self-compiled compiler to ensure consistent code generation.

### Chapter 3.1 - Building The Host's Toolchain

Before we can build a cross toolchain, we need to build a host toolchain.  This
is so we can build a cross toolchain that was compiled by itself, which gives
more consistent behavior, and have no worry about the quality of our system's
toolchain.

First, we need to build `binutils`, which can be done with the following:

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-binutils-stage1
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-binutils-stage1

../../sources/binutils-*/configure \
   --disable-dependency-tracking \
   --host=${TUX64_TARGET_HOST} \
   --prefix=${TUX64_BUILD_ROOT}/tools \
   --program-prefix=${TUX64_TARGET_HOST}- \
   CFLAGS="${TUX64_CFLAGS_HOST}" \
   CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
   ASFLAGS="${TUX64_ASFLAGS_HOST}" \
   LDFLAGS="${TUX64_LDFLAGS_HOST}" \
   --disable-werror \
   --enable-host-pie \
   --enable-lto
```

This will create our build directory and configure `binutils`.

Next, we need to build `binutils` with the following:

```
make -j${TUX64_MAKEOPTS}
```

This will compile `binutils`, but not install it to our tools directory.  We can
do that with the following:

```
make -j${TUX64_MAKEOPTS} install-strip
```

This will strip the binaries and then install them.  We strip the binaries
because there is almost no use for program symbols and unused sections for
end-users, and they end up wasting disk space.

We now have our stage-1 `binutils` installed.  Next we configure and build
`gcc`.  This process takes 3 stages:

* (stage1) Compile `gcc` with the system's toolchain
* (stage2) Compile `gcc` with the stage1 build
* (stage3) Compile `gcc` with the stage2 build, and compare stage2 and stage3 to make sure they're the same

This achieves a toolchain which was effectively compiled by itself.  The
downside is it takes much longer to build, likely hours.  Since it takes so
long, let's not waste time and get to building!

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-gcc
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-gcc

../../sources/gcc-*/configure \
   --disable-dependency-tracking \
   --host=${TUX64_TARGET_HOST} \
   --prefix=${TUX64_BUILD_ROOT}/tools \
   CFLAGS="${TUX64_CFLAGS_HOST} -fno-lto" \
   CXXFLAGS="${TUX64_CXXFLAGS_HOST} -fno-lto" \
   ASFLAGS="${TUX64_ASFLAGS_HOST}" \
   LDFLAGS="${TUX64_LDFLAGS_HOST} -fno-lto" \
   --disable-werror \
   --enable-host-pie \
   --enable-lto \
   --enable-bootstrap \
   --enable-languages=c,c++

make -j${TUX64_MAKEOPTS} all
make -j${TUX64_MAKEOPTS} install-strip
```

Once `gcc` finally finishes building and installing, we will have a bootstrapped
host toolchain.  Lastly, we need to recompile `binutils` using our fresh host
toolchain:

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-binutils-stage2
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-binutils-stage2

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/binutils-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      --program-prefix=${TUX64_TARGET_HOST}- \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}" \
      --disable-werror \
      --enable-host-pie \
      --enable-lto
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

Note that the `configure` step also sets environment variables for the toolchain
using `usetoolchain.sh`.  Also note that this has to be wrapped using shell slop
to prevent the environment variables from leaking to other builds, which could
cause major problems.

If successful so far, you now have a fully bootstrapped host toolchain!

### Chapter 3.2 - Building The Bootloader's Toolchain

First we need to build `binutils` for the Nintendo 64's bootloader toolchain.
Note that we use our bootstrapped host toolchain to build the cross-toolchain,
however we are still building code to run on the host right now, so we use the
host's compiler flags.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_BOOTLOADER}-binutils
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_BOOTLOADER}-binutils

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/binutils-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_BOOTLOADER} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      --program-prefix=${TUX64_TARGET_N64_BOOTLOADER}- \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}" \
      --disable-werror \
      --enable-host-pie \
      --enable-lto \
      --with-cpu=mips64vr4300
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

Next, we will build `gcc`.  We don't need to build the full toolchain since we
will only be using it to build the bootloader, which only requires the compiler
runtime for basic operations.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_BOOTLOADER}-gcc
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_BOOTLOADER}-gcc

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/gcc-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_BOOTLOADER} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      CFLAGS="${TUX64_CFLAGS_HOST} -fno-lto" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST} -fno-lto" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST} -fno-lto" \
      CFLAGS_FOR_TARGET="${TUX64_CFLAGS_N64_BOOTLOADER} -fno-lto" \
      CXXFLAGS_FOR_TARGET="${TUX64_CXXFLAGS_N64_BOOTLOADER} -fno-lto" \
      ASFLAGS_FOR_TARGET="${TUX64_ASFLAGS_N64_BOOTLOADER}" \
      LDFLAGS_FOR_TARGET="${TUX64_LDFLAGS_N64_BOOTLOADER} -fno-lto" \
      --disable-werror \
      --enable-host-pie \
      --enable-lto \
      --disable-bootstrap \
      --enable-languages=c \
      --with-arch=vr4300 \
      --with-tune=vr4300 \
      --with-abi=o64 \
      --disable-multilib \
      --without-headers \
      --disable-shared \
      --disable-libssp
)

make -j${TUX64_MAKEOPTS} all-gcc
make -j${TUX64_MAKEOPTS} install-strip-gcc
make -j${TUX64_MAKEOPTS} all-target-libgcc
make -j${TUX64_MAKEOPTS} install-target-libgcc
```

We now have all the tools required to build the bootloader!

### Chapter 3.3 - Building The Nintendo 64 Kernel and Userspace Toolchain

Now we need to build the toolchail which will compile the Nintendo 64's Linux
kernel and userspace software.  This is the most tedious part, because we have
to build `musl` to build the toolchain, but we need the toolchain to build
`musl`.

First we build `binutils` as we did for the previous parts, but now targetting
Linux instead of the bootloader.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-binutils
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-binutils

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/binutils-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      --program-prefix=${TUX64_TARGET_N64_LINUX}- \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}" \
      --disable-werror \
      --enable-host-pie \
      --enable-lto \
      --with-cpu=mips64vr4300
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

Next, we build the stage-1 `gcc`.  This will be used to compile `musl`, which
whill then be used to build the full `gcc` toolchain.  We only want to build and
install the compiler itself, as `libgcc` depends on `musl`.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-gcc-stage1
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-gcc-stage1

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/gcc-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      CFLAGS="${TUX64_CFLAGS_HOST} -fno-lto" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST} -fno-lto" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST} -fno-lto" \
      CFLAGS_FOR_TARGET="${TUX64_CFLAGS_N64_LINUX} -fno-lto" \
      CXXFLAGS_FOR_TARGET="${TUX64_CXXFLAGS_N64_LINUX} -fno-lto" \
      ASFLAGS_FOR_TARGET="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS_FOR_TARGET="${TUX64_LDFLAGS_N64_LINUX} -fno-lto" \
      --disable-werror \
      --enable-host-pie \
      --enable-lto \
      --disable-bootstrap \
      --enable-languages=c,c++ \
      --with-arch=vr4300 \
      --with-tune=vr4300 \
      --with-abi=o64 \
      --disable-multilib \
      --disable-shared \
      --disable-libssp \
      --disable-libgomp \
      --disable-libsanitizer \
      --disable-libatomic
)

make -j${TUX64_MAKEOPTS} all-gcc
make -j${TUX64_MAKEOPTS} install-strip-gcc
```

Once this completes, we need to configure `musl` and install its headers and
compiler runtime.  We will build a shared object for `musl` once our toolchain
is complete.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-musl-stage1
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-musl-stage1

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}
   ../../sources/musl-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX} \
      CFLAGS="${TUX64_CFLAGS_N64_LINUX} -fno-lto" \
      ASFLAGS="${TUX64_ASFLAGS_N64_LINUX}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_LINUX} -fno-lto" \
      --disable-shared
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

Now we will return to the stage-1 `gcc` build to build the rest of the
toolchain.

```
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-gcc-stage1
make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

We now have our stage-1 toolchain.  We will use this to build the full-featured
stage-2 toolchain.

First, we will install kernel headers.  These are required for various
supporting `gcc` libraries.

```
mkdir ${TUX64_BUILD_ROOT}/builds/linux-headers
cd ${TUX64_BUILD_ROOT}/builds/linux-headers

${TUX64_BUILD_ROOT}/scripts/kernel-make.sh headers_install
```

TODO: Build full stage-2 `gcc` and `musl`.  We want to build both static and
shared objects and also make use of LTO.  We also want to build the libraries
above we disabled.

We will now proceed to [building userspace software](04-building-userspace-software.md).

