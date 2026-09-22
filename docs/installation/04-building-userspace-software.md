# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 3 - Building The Toolchains](03-building-the-toolchains.md)
* Next Page: [Chapter 5 - Building The Kernel](05-building-the-kernel.md)

## Chapter 4 - Building Userspace Software

We will now build userspace programs and libraries which are used on both the
host and Nintendo 64.

### Chapter 4.1 - Building `tux64-lib`

`tux64-lib` is a library for Tux64 programs which contains globally-shared
functionality.  This is required for all platforms which will run Tux64-specific
programs.

First we'll build `tux64-lib` for our host platform:

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-tux64-lib
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-tux64-lib

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/tux64-*/lib/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}" \
      --enable-platform-cpu-signed-integer-format-twos-complement \
      --enable-log \
      --enable-log-ansi
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

Next, we'll build `tux64-lib` for the bootloader:

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_BOOTLOADER}-tux64-lib
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_BOOTLOADER}-tux64-lib

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_BOOTLOADER}
   ../../sources/tux64-*/lib/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_BOOTLOADER} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER} \
      CFLAGS="${TUX64_CFLAGS_N64_BOOTLOADER}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_BOOTLOADER}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_BOOTLOADER}" \
      --enable-platform-cpu-signed-integer-format-twos-complement \
      --enable-platform-mips-n64 \
      --enable-platform-mips-vr4300
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

Finally, we'll build `tux64-lib` for the Nintendo 64 userspace:

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

### Chapter 4.2 - Building `tux64-mkrom`

`tux64-mkrom` is a tool used to create the final bootable ROM image which will
run on the Nintendo 64.  This will be needed near the end of the installation.

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-mkrom
cd ${TUX64_BUILD_ROOT}/builds/tux64-mkrom

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/tux64-*/mkrom/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}"
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

### Chapter 4.3 - Building `tux64-rescompiler`

`tux64-rescompiler` is a collection of tools used to convert resources to
embedded binary formats for use with `tux64-boot`.  Tools built by this package
are a hard build requirement for `tux64-boot`.

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-rescompiler
cd ${TUX64_BUILD_ROOT}/builds/tux64-rescompiler

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/tux64-*/rescompiler/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}"
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

### Chapter 4.4 - Building `e2fsprogs`

`e2fsprogs` provides utilities for ext2, ext3, and ext4 files systems.  This
will be used later to create a root filesystem image with `mke2fs`.

```
mkdir ${TUX64_BUILD_ROOT}/builds/e2fsprogs
cd ${TUX64_BUILD_ROOT}/builds/e2fsprogs

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/e2fsprogs-*/configure \
      --host=${TUX64_TARGET_HOST} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      --sbindir=${TUX64_BUILD_ROOT}/tools/bin \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}" \
      --enable-lto \
      --enable-year2038 \
      --disable-debugfs \
      --disable-imager \
      --disable-resizer \
      --disable-defrag \
      --disable-uuidd \
      --disable-fuse2fs
)

make -j${TUX64_MAKEOPTS}
cp misc/mke2fs ${TUX64_BUILD_ROOT}/tools/bin/mke2fs
${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}-strip ${TUX64_BUILD_ROOT}/tools/bin/mke2fs
```

Note that we manually install `mke2fs`.  This is because `e2fsprogs` contains
many tools, but we only need `mke2fs`.  In fact, attempting to install all tools
will give errors due to `e2fsprogs` attempting to install udev rules to the
host's root filesystem.

We will now proceed to [building the kernel](05-building-the-kernel.md).

