## Introduction

Welcome to the Tux64 development guide!  This will help those looking to do their own work on Tux64, whether you're hacking on the kernel for fun or are looking to contribute.

## Changes to ```buildconf.sh```

It's recommended to make the following changes to ```buildconf.sh``` to aid in debugging:

| Name | Value |
|------|-------|
| TUX64_CFLAGS_HOST | -pipe -march=native -O0 -g -fsanitize=undefined |
| TUX64_LDFLAGS_HOST | -g |
| TUX64_CFLAGS_N64_COMMON |  -pipe -march=vr4300 -mfix4300 -O0 -g |
| TUX64_LDFLAGS_N64_COMMON | -g |
| TUX64_CFLAGS_N64_BOOTLOADER | ${TUX64_CFLAGS_N64_COMMON} -mabi=o64 |

You may either overwrite ```buildconf.sh```, or save the modified version to a new file, such as ```buildconf-dev.sh``` so you may use the development ```buildconf-dev.sh``` only for Tux64 code.  This may be desirable for compiling external software and libraries with optimized compiler flags, while still retaining enhanced debug support for internal code.

## Enabling dependency tracking

If you look closely, the ```configure``` commands from the Installation Guide include ```--disable-dependency-tracking```.  This is done to skip all dependency tracking for sources files in order to speed up compilation.  Dependency tracking is used to detect if a file's dependencies are modified, and if so, recompile that file.  For example, if a C header file is modified, all C source files which include that header will be recompiled.  For end-users, this functionality serves no purpose but to slow down compilation.  However, for developers, this is a must if you don't want to constantly run ```make clean``` before each recompile.

To enable dependency tracking, simply replace all instances of ```---disable-dependency-tracking``` with ```--enable-dependency-tracking```, or omit both as the default is to enable dependency tracking.

## Enabling ```tux64-lib``` log origin

```tux64-lib```'s logging functions include support to include the source file and line number each log message originates from.  This can be useful for development to quickly track down specific log messages without needing a debugger attached.

To do this, reconfigure ```tux64-lib``` with ```---enable-log-origin```.

# Enabling debug assertions

`tux64-boot` can be optionally configured with `--enable-debug`.  This enables some safety checks which can catch programming mistakes at the cost of some runtime overhead.

This may be useful for bootloader development, where simple programming mistakes will cause the bootloader to safely halt instead of violently exploding, aiding in debugging.

## Enabling boot status codes

`tux64-boot` can be optionally configured with `--enable-status`.  This will write status code information to a fixed memory address at the end of RSP DMEM, specifically 8 bytes to `0xa4000ff8`.  The bytes are ASCII characters which spell out `STAGEn:p`, where "n" is the bootloader stage and "p" is the location last reached within the relevant bootloader stage.

These may be useful for extremely low-level debugging, such as debugging very early boot failures on real hardware before the remote debugger is available by probing the SysAD bus with a logic analyzer.  Otherwise, these serve no purpose and can be safely left disabled.

## Enabling dumping of register file and status codes to cartridge SRAM.

`tux64-boot` can be optionally configured with `--enable-dump-to-sram`.  This will add the ability to dump the entire general-purpose register file and current status code to cartridge SRAM with the macro `TUX64_BOOT_STAGE0_DUMP_TO_SRAM`.

This can be used to aid in stage-0 debugging without the need of a logic analyzer as detailed above.  However, it will require a flashcart with the ability to dump save files, such as a SummerCart64.

Note that this configuration option will break the brute-forced IPL2 checksum value.  This may be a problem with some flashcarts and require computing a new checksum value, but your mileage may vary.

## Bootloader and kernel debugging

When debugging the bootloader and kernel, you have the choice of either using an emulator running on the host system, or remotely debugging a real Nintendo 64.  Information for debugging both will be provided.

### Obtaining sources

The following packages are required for debugging both with an emulator and real hardware.

The process of obtaining sources is the same as the Installation Guide.

| Package | Version | Notes |
|---------|---------|-------|
| [gdb](https://www.sourceware.org/gdb/) | 17.2 | |
| [ares](https://ares-emu.net/) | v148 | |

### Building GDB

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

### Debugging in an emulator

It's recommended to use Ares with the N64 core for development, as it's an accurate emulator with support for development features, such as viewing memory, monitoring various I/O components, and most importantly - remote debugging via GDB.  It's open-source and cross-platform, and there's no wonderful features like adding a donation pop-up which blocks using the emulator for 30 seconds.  We will show how to attach GDB and debug as early as the first instruction of the boot process, complete with full symbolic debugging.

#### Obtaining sources

These packages are only required when debugging in an emulator.

| Package | Version | Notes |
|---------|---------|-------|
| [ares](https://ares-emu.net/) | v148 | |

#### Building Ares

If you are looking to debug or develop the bootloader or write kernel patches, there are source patches you can apply which adds options to aid in bootloader and kernel development.  These can be applied with the following:

```
cd ${TUX64_BUILD_ROOT}/sources/ares-*
git apply ${TUX64_BUILD_ROOT}/sources/tux64-*/patches/ares-*.patch
```

If you aren't interested in bootloader or kernel development, the above step may be skipped.

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

#### Configuring Linux for debugging

We now have to enable a few kernel configuration options for kernel debugging.

```
Kernel hacking  --->
  Compile-time checks and compiler options  --->
    Debug information  --->
      (X) Rely on the toolchain's implicit default DWARF version

    [*] Provide GDB scripts for kernel debugging
```

Now recompile and install the kernel, as well as rebuild the ROM image as detailed in the installation guide.

#### Configuring Ares for debugging

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

#### Attaching GDB for bootloader debugging

Launch GDB with the following command:

```
${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_BOOTLOADER}-gdb
```

Connect to Ares with the following GDB command:

```
target remote localhost:(Ares GDB Server Port, default 9123)
```

This will attach to Ares's GDB server, debugging the emulated N64.  To debug the bootloader, type the following GDB command, where 'N' is the bootloader stage you wish to debug:

```
add-symbol-file ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/share/tux64-boot/stageN.sym
```

We can repeat the above command to load symbols from as many stages as we like.  This can be useful for debugging multiple bootloader stages at the same time.

Note that shell environment variable substitution doesn't work here, so these will need to be resolved manually.  You should now be able to set breakpoints, list functions, disassemble, and all the usual GDB fanciness.

Alternatively, you can load the symbol file when launching GDB:

```
${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_BOOTLOADER}-gdb \
   -s ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/share/tux64-boot/stageN.sym
```

This has the advantage of being an easily copy-pasted command, but only one symbol file can be loaded at a time.  Adding additional symbol files will require the above `add-symbol-file` GDB command.

To start the emulator, type ```continue```, or ```c``` for short in GDB, then untick Tools > Pause Emulation.

#### Attaching GDB for kernel debugging

Launch GDB with the following command:

```
${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_LINUX}-gdb \
    ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/vmlinux
```

Debugging now follows the same as for the bootloader.

TODO: install and load GDB scripts.

### Debugging on real hardware

It is also possible to debug on real hardware.  However, this will require specialized hardware that can cost a significant amount of cash.

This guide will document the SummerCart64 flashcart, an open-source N64 flashcart with remote debugging capabilities.  It should provide the most accessible development environment, thus this guide will document the debugging process with the SummerCart64.  It may be possible to use other flashcarts, but it will not be documented here.

#### Obtaining sources

| Package | Version | Notes |
|---------|---------|-------|
| [SummerCart64](https://github.com/Polprzewodnikowy/SummerCart64/) | 2.20.2 | |

#### Building sc64deployer

To interface with the SummerCart64 from the host machine, we will need to build `sc64deployer`.

```
mkdir ${TUX64_BUILD_ROOT}/builds/sc64deployer
cd ${TUX64_BUILD_ROOT}/builds/sc64deployer

cargo build --release \
   -j${TUX64_MAKEOPTS} \
   --manifest-path=$(ls ${TUX64_BUILD_ROOT}/sources/SummerCart64-*/sw/deployer/Cargo.toml) \
   --target-dir=$(pwd)

cp release/sc64deployer ${TUX64_BUILD_ROOT}/tools/bin/
```

#### Uploading ROM images

You can upload `tux64.n64` with the following command, assuming `tux64.n64` is located in the default build directory from the Installation Guide:

```
sudo ${TUX64_BUILD_ROOT}/tools/bin/sc64deployer upload \
    --direct ${TUX64_BUILD_ROOT}/builds/tux64-rom-image/tux64.n64 \
    --save-type sram
```

We use `--save-type sram` so we can do SRAM-based remote debugging for the stage-0 bootloader.

#### SRAM-based debugging for the stage-0 bootloader

As stage-0 is so early in the boot process, we don't have our remote debugger available yet.  The only way to debug on real hardware without specialized console modifications or logic analyzers is by dumping the register file and status codes to cartridge SRAM, then downloading and decoding SRAM on the host.

To do this, make sure `tux64-boot` is configured with `--enable-dump-to-sram`, otherwise this will not work.

To debug stage-0, insert a call to the assembler macro `TUX64_BOOT_STAGE0_DUMP_TO_SRAM` into the relevant part of code.  When called, the CPU will dump its register files and status codes to cartridge SRAM.  Note that this will break the brute-forced IPL2 checksum values.  From my testing, this isn't a problem on the SummerCart64, but other flashcarts may give problems.

Once you've inserted your call to `TUX64_BOOT_STAGE0_DUMP_TO_SRAM`, recompile the bootloader, and rebuild the ROM image.  Upload the ROM image to the console via the previously mentioned steps, and wait a few seconds for the console to execute the macro invocation.  We now retrieve the SRAM dump with the following command:

```
sudo ${TUX64_BUILD_ROOT}/tools/bin/sc64deployer download save sram-dump.bin
```

TODO: create a tool to decode the SRAM dump and present it in a more readable form.

#### Attaching GDB for bootloader debugging

TODO: implement and document once we get stage-0 working.

#### Attaching GDB for kernel debugging

TODO: implement and document once we get stage-0 working.  For this, we will need serial support and KGDB.

## Recalculating the ```tux64-boot``` stage-0 CIC data

If you are an end-user, you should not have to do this.  However, if you modify a single byte of the stage-0 binary, it will cause the checksum calculated by the PIF to be incorrect, thus the console won't boot.  You must recalculate the CIC data in order to modify the stage-0 bootloader.

### Obtaining sources

The following tools are needed to re-calculate the stage-0 CIC data:

| Package | Version | Notes |
|---------|---------|-------|
| [ipl3hasher-new](https://github.com/Polprzewodnikowy/ipl3hasher-new) | 1.3.0 | This requires a Rust toolchain to be installed. |

### Building ipl3hasher-new

```
mkdir ${TUX64_BUILD_ROOT}/builds/ipl3hasher-new
cd ${TUX64_BUILD_ROOT}/builds/ipl3hasher-new

cargo build --release \
   -j${TUX64_MAKEOPTS} \
   --manifest-path=$(ls ${TUX64_BUILD_ROOT}/sources/ipl3hasher-new-*/Cargo.toml) \
   --target-dir=$(pwd)

cp release/ipl3hasher-new ${TUX64_BUILD_ROOT}/tools/bin/
```

You can now run ipl3hasher-new with the following command:

```
${TUX64_BUILD_ROOT}/tools/bin/ipl3hasher-new
```

### Calculating CIC data

We will now use ipl3hasher-new to brute-force the CIC data for force the stage-0 CIC checksum to match.
For now, we target the `CIC-6102` only, but future versions may target multiple CIC revisions.

```
${TUX64_BUILD_ROOT}/tools/bin/ipl3hasher-new \
   ${TUX64_BUILD_ROOT}/builds/tux64-rom-image/*.n64 \
   --cic 6102
```

When complete, you'll see output which looks like the following:

```
Found collision: Y=???????? X=????????
```

The `Y` value is the first 32-bit word for `.cicdata`, and `X` is the second.

