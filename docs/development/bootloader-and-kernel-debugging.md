# Tux64 Development Guide
<img src="../logo.png" width="156" height="156"/>

## Bootloader and Kernel Debugging

When debugging the bootloader and kernel, you have the choice of either using an
emulator running on the host system, or remotely debugging a real Nintendo 64.
Information for debugging both will be provided.

### Obtaining Sources

The following packages are required for debugging both with an emulator and real
hardware.

The process of obtaining sources is the same as the installation guide.

| Package | Version | Notes |
|---------|---------|-------|
| [gdb](https://www.sourceware.org/gdb/) | 17.2 | |

### Building `gdb`

First, we will build `gdb` targetting the bootloader.  If you don't intend on
debugging the bootloader, you can skip this step.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_BOOTLOADER}-gdb
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_BOOTLOADER}-gdb

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/gdb-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_BOOTLOADER} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      --program-prefix=${TUX64_TARGET_N64_BOOTLOADER}- \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}" \
      --enable-host-pie \
      --enable-lto
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

Next, we will build `gdb` targetting Linux.  This will be used to debug the
Linux kernel as well as userspace programs.  If you don't intend on debugging
the kernel or userspace programs, you can skip this step.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-gdb
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-gdb

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/gdb-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_LINUX} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      --program-prefix=${TUX64_TARGET_N64_LINUX}- \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}" \
      --enable-host-pie \
      --enable-lto
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

### Debugging in an Emulator

It's recommended to use Ares with the N64 core for development, as it's an
accurate emulator with support for development features, such as viewing memory,
monitoring various I/O components, and most importantly - remote debugging via
GDB.  It's open-source and cross-platform, and there's no wonderful features
like adding a donation pop-up which blocks using the emulator for 30 seconds.
We will show how to attach GDB and debug as early as the first instruction of
the boot process, complete with full symbolic debugging.

#### Obtaining Sources

These packages are only required when debugging in an emulator.

| Package | Version | Notes |
|---------|---------|-------|
| [ares](https://ares-emu.net/) | v148 | |

#### Building Ares

While you can use base Ares for debugging, there are source patches you can
apply which adds options to aid in bootloader and kernel development.  These can
be applied with the following:

```
cd ${TUX64_BUILD_ROOT}/sources/ares-*
git apply ${TUX64_BUILD_ROOT}/sources/tux64-*/patches/ares-*.patch
```

Next, we build Ares with the following:

```
mkdir ${TUX64_BUILD_ROOT}/builds/ares
cd ${TUX64_BUILD_ROOT}/builds/ares

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   cmake \
      -DCMAKE_INSTALL_PREFIX=${TUX64_BUILD_ROOT}/tools \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_FLAGS="${TUX64_CFLAGS_HOST} -s" \
      -DCMAKE_CXX_FLAGS="${TUX64_CXXFLAGS_HOST} -s" \
      -DARES_CORES=n64 \
      -S ../../sources/ares-*
)

make C_FLAGS="${TUX64_CFLAGS_HOST}" CXX_FLAGS="${TUX64_CXXFLAGS_HOST}" -j${TUX64_MAKEOPTS}
make C_FLAGS="${TUX64_CFLAGS_HOST}" CXX_FLAGS="${TUX64_CXXFLAGS_HOST}" -j${TUX64_MAKEOPTS} install
```

You can now run Ares with the following command:

```
${TUX64_BUILD_ROOT}/tools/bin/ares
```

#### Configuring Ares for Debugging

Before attempting to attach GDB, you should do the following:
   * Disable the recompiler and use the interpreter under Settings > Options > Force Interpreter
   * Enable GDB server under Settings > Debug > Enabled
   * Enable GDB IPv4 support under Settings > Debug > Use IPv4
   * Set on-focus-loss behavior to "Block input" under Settings > Drivers > When focus is lost
   * Launch the GDB server on boot under Settings > Boot Options > Launch Tracer

Additionally, if you're doing bootloader or kernel development and applied the above patches, the following options might be interesting:
   * Disable RDRAM pre-initialization under Settings > Options > Nintendo 64 Settings > Pre-Initialize RDRAM
   * Disable IPL2 checksum enforcement under Settings > Options > Nintendo 64 Settings > Enforce IPL2 Checksum
   * Enable kernel `printk()` tracing under Tools > Tracer > PI Serial

#### Attaching GDB

Launch GDB with the following command:

```
${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_BOOTLOADER}-gdb
```

Connect to Ares with the following GDB command:

```
target remote localhost:(Ares GDB Server Port, default 9123)
```

This will attach to Ares's GDB server, debugging the emulated N64.

To debug the bootloader, type the following GDB command, where 'N' is the
bootloader stage you wish to debug:

```
add-symbol-file ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/share/tux64-boot/stageN.sym
```

We can repeat the above command to load symbols from as many stages as we like.
This can be useful for debugging multiple bootloader stages at the same time.

Note that shell environment variable substitution doesn't work here, so these
will need to be resolved manually.  You should now be able to set breakpoints,
list functions, disassemble, and all the usual GDB fanciness.

Alternatively, you can load the symbol file when launching GDB:

```
${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_BOOTLOADER}-gdb \
   -s ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/share/tux64-boot/stageN.sym
```

This has the advantage of being an easily copy-pasted command, but only one
symbol file can be loaded at a time.  Adding additional symbol files will
require the above `add-symbol-file` GDB command.

To debug the kernel, attach the kernel image to GDB with the following command:

```
add-symbol-file ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/vmlinux
```

Alternatively, you can load the kernel symbols when launching GDB:

```
${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}-gdb \
    ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/vmlinux
```

To start the emulator, type `continue`, or `c` for short in GDB, then untick
Tools > Pause Emulation.

### Debugging on Real Hardware

It is also possible to debug on real hardware.  However, this will require
specialized hardware that can cost a significant amount of cash or simply be
unavailable.

This guide will document the SummerCart64 flashcart, an open-source N64
flashcart with remote debugging capabilities.  It should provide the most
accessible development environment, thus this guide will document the debugging
process with the SummerCart64.  It may be possible to use other flashcarts, but
it will not be documented here.

#### Obtaining Sources

| Package | Version | Notes |
|---------|---------|-------|
| [SummerCart64](https://github.com/Polprzewodnikowy/SummerCart64/) | 2.20.2 | |

#### Building `tux64-sramdumper`

`tux64-sramdumper` is a tool used to parse `tux64-boot` stage-0 SRAM dumps into
a human-readable form.  If you are looking to debug the stage-0 bootloader on
real hardware, you may want to build this package.  Otherwise, you may safely
skips this step.

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-sramdumper
cd ${TUX64_BUILD_ROOT}/builds/tux64-sramdumper

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/tux64-*/sramdumper/configure \
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

#### Building `sc64deployer`

To interface with the SummerCart64 from the host machine, we will need to build
`sc64deployer`.

```
mkdir ${TUX64_BUILD_ROOT}/builds/sc64deployer
cd ${TUX64_BUILD_ROOT}/builds/sc64deployer

cargo build --release \
   -j${TUX64_MAKEOPTS} \
   --manifest-path=$(ls ${TUX64_BUILD_ROOT}/sources/SummerCart64-*/sw/deployer/Cargo.toml) \
   --target-dir=$(pwd)

cp release/sc64deployer ${TUX64_BUILD_ROOT}/tools/bin/
```

#### Uploading ROM Images

You can upload `tux64.n64` with the following command, assuming `tux64.n64` is
located in the default build directory from the installation guide:

```
sudo ${TUX64_BUILD_ROOT}/tools/bin/sc64deployer upload \
    --direct ${TUX64_BUILD_ROOT}/builds/tux64-rom-image/tux64.n64 \
    --save-type sram
```

We use `--save-type sram` so we can do SRAM-based remote debugging for the
stage-0 bootloader.

#### SRAM-based debugging for the stage-0 bootloader

As stage-0 is so early in the boot process, we don't have our remote debugger
available yet.  The only way to debug on real hardware without specialized
console modifications or logic analyzers is by dumping the register file and
status codes to cartridge SRAM, then downloading and decoding SRAM on the host.

To do this, make sure `tux64-boot` is configured with `--enable-dump-to-sram`,
otherwise this will not work.

To debug stage-0, insert a call to the assembler macro
`TUX64_BOOT_STAGE0_DUMP_TO_SRAM` into the relevant part of code.  When called,
the CPU will dump its register files and status codes to cartridge SRAM.  Note
that this will break the brute-forced IPL2 checksum values.  From my testing,
this isn't a problem on the SummerCart64, but other flashcarts may give
problems.

Once you've inserted your call to `TUX64_BOOT_STAGE0_DUMP_TO_SRAM`, recompile
the bootloader, and rebuild the ROM image.  Upload the ROM image to the console
via the previously mentioned steps, and wait a few seconds for the console to
execute the macro invocation.  We now retrieve the SRAM dump with the following
command:

```
sudo ${TUX64_BUILD_ROOT}/tools/bin/sc64deployer download save sram-dump.bin
```

This will give us the raw memory dump.  We now convert this into a
human-readable from with `tux64-sramdumper`:

```
${TUX64_BUILD_ROOT}/tools/bin/tux64-sramdumper \
   --input sram-dump.bin \
   --output sram-dump.txt
```

You can now inspect status codes and the register file in `sram-dump.txt`.

If you would like a one-liner command to do all of the above at once for faster
debugging, here it is:

```
sudo ${TUX64_BUILD_ROOT}/tools/bin/sc64deployer download save sram-dump.bin && \
${TUX64_BUILD_ROOT}/tools/bin/tux64-sramdumper \
   --input sram-dump.bin \
   --output sram-dump.txt && \
rm -f sram-dump.bin && \
cat sram-dump.txt
```

#### Attaching GDB for Bootloader Debugging

TODO: implement and document once we get stage-0 working.

#### Attaching GDB for Kernel Debugging

TODO: implement and document once we get stage-0 working.  For this, we will need serial support and KGDB.

