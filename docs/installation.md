## Introduction

Welcome to Tux64!  This distribution of Linux aims to bring a minimal, but hopefully usable terminal Linux environment to the Nintendo 64.  This guide will start with source code for individual packages and end with a bootable ROM image which contains the entire operating system.

This is a long and tedious process, involving bootstrapping multiple compiler toolchains from source, configuring a Linux kernel, and building many other userspace applications.  While this guide aims to make this process as painless as possible, it's still a difficult process intended for experienced Linux users.  For reference, this guide is about the same level as Linux from Scratch.  It is recommended to be comfortable with installing Gentoo Linux, as much of the general process of installing Gentoo Linux will be seen here, minus the package manager and "stage" files.  Additionally, having experience with C or C++ toolchains and build systems, such a GCC and Autotools, may prove invaluable, especially if you encounter issues during the build process.

## Chapter 0 - Prerequisites

We will be building most of our software using GNU Autotools, GNU Make, and other free software.  If building on Windows, it's recommended to use Windows Subsystem for Linux and perform compilation within there.  If on Linux or some flavor of BSD, you likely already have all the necessary tools to build Tux64.  If not, you may have to download your distribution's "developer" toolchain.

For example, on Debian, Ubuntu, Mint, etc., you would type the following:

```
apt-get install build-essential
```

Or if you're on Arch Linux or any of its derivatives, you would type the following:

```
pacman -S base-devel
```

Additionally, if you would like to run your Tux64 build, you will need a way to run Nintendo 64 ROMs.  You could either choose to use an emulator, or run it on real hardware.  For real hardware, you will need a flashcart which can run ROMs from a file, such as an EverDrive64 or SummerCart64.  For an emulator, you will need a modern LLE emulator.  This excludes emulators such as Project64 and Mupen64(Plus), as these are designed to run commercial games with high performance, at the cost of emulation accuracy.  I recommend either [Ares](https://ares-emu.net/) or [dgb-n64](https://github.com/Dillonb/n64).

## Chapter 1 - Setup

We will create a directory which will store all files relating to Tux64.  This will allow us to isolate our build from the rest of the system so we can simply delete the entire directory once we finish our installation.  The recommended directory structure, which will be assumed for the rest of the installation guide, is as follows:

```
(TUX64 BUILD ROOT)/
   sources/
   builds/
   scripts/
   tools/
   initramfs/
   rootfs/
```

This can be done with the following command:

```
cd [TUX64 BUILD ROOT]
mkdir sources builds scripts tools initramfs rootfs
```

A description of each directory is provided here:

| Directory  | Description |
| ---------- | ----------- |
| sources | Source code for all our packages |
| builds | Build artifacts for compiled versions of packages |
| scripts | Helper scripts to aid the install process |
| tools | Installed tools and libraries for the toolchains |
| initramfs | Tux64 initramfs filesystem |
| rootfs | Tux64 root filesystem |

## Chapter 2 - Obtaining the Sources

Before we can start building the required packages, we need to download their sources (short for source-code tarballs, or just source-code).  Many of the packages also offer signatures.  **It is highly recommended to verify signatures when possible, otherwise you may run malware on your computer.**  Even though we are building software to run on the Nintendo 64, we are still executing builds _scripts_, which execute code on our host machine.  If we don't verify signatures, this can leave us open to hackers providing malicious sources bundled with malware in the build scripts.

### Chapter 2.1 - Verifying Authenticity of Sources

To verify signatures for a downloaded package, you will need GNU Privacy Guard, otherwise known as GPG.  Once you have GPG, you will need three components:  The package sources, the signature for the package sources, and the signer's public key.

The signature for the package sources are often bundled right next to the sources themselves, but sometimes it may be a little tricky to find.  Some software doesn't provide digital signatures at all, instead relying on HTTPS or other forms on protected transmission to provide data integrity.

The signer's public key can be a little more tricky to locate.  Often, either the project's website will offer their public key, or the individual signer for the release will have a personal site which offers their public key.  These can be tricky to find, but it's worth protecting yourself against hackers when possible.  You can also attempt to search for public keys from a key server with the following:

```
gpg --locate-keys [QUERY]
```

For example, to find public keys for Linux maintainers, you would type the following:

```
gpg --locate-keys @kernel.org
```

This will give output similar to the following:

```
pub   rsa4096 2011-09-23 [SC]
      647F28654894E3BD457199BE38DBBDC86092693E
uid           [ unknown] Greg Kroah-Hartman <gregkh@linuxfoundation.org>
uid           [ unknown] Greg Kroah-Hartman <gregkh@kernel.org>
uid           [ unknown] Greg Kroah-Hartman (Linux kernel stable release signing key) <greg@kroah.com>
sub   rsa4096 2011-09-23 [E]

pub   rsa2048 2011-09-20 [SC]
      ABAF11C65A2970B130ABE3C479BE3E4300411886
uid           [ unknown] Linus Torvalds <torvalds@kernel.org>
sub   rsa2048 2011-09-20 [E]
```

To import a signer's public key from a file, type the following:

```
gpg --import [FILE]
```

To import a signer's public key from a key server, type the following:

```
gpg --recv-keys [FINGERPRINT]
```

Using the above example again, if we wanted to import Greg Kroah-Hartman's public key, we would type the following:

```
gpg --recv-keys 647F28654894E3BD457199BE38DBBDC86092693E
```

### Chapter 2.2 - Required Packages

Provided is a table of all the required sources, and their recommended version.  While it is likely possible to use packages with different versions or external patches, we don't guarantee compatibility with the rest of the packages, and will likely require extra work to make it work.

Every package listed should be stored uncompressed inside of `sources/`, with the format `[NAME]-[VERSION]`.

| Package | Version | Notes |
|---------|---------|-------|
| [tux64](https://github.com/bradleycha/tux64/) | master | Currently no stable release.  Use `git` to clone the latest version of the 'master' branch.  If cloning directly from GitHub, you must run ```autoreconf -i```  for each sub-project which contains a GNU Autoconf script (```configure.ac```) |
| [binutils](https://www.gnu.org/software/binutils/) | 2.44 | |
| [gcc](https://gcc.gnu.org/) | 15.1.0 | |
| [musl](https://musl.libc.org/) | 1.2.5 | |
| [linux](https://kernel.org/) | 6.12.35 | Signatures should be checked against the uncompressed tarball, not the compressed one (i.e. `xz --decompress linux-*.tar.xz && gpg --verify linux-*.tar.sign`). |

## Chapter 3 - Building the Toolchains

In order to build code for the Nintendo 64, we need to create a toolchain which can build such code.  In fact, we need multiple toolchains, as we are building both binaries to run under Linux and flat code to run without an operating system.  We also want to use the 3-stage bootstrapping process to get a self-compiled compiler to ensure consistent code generation.

### Chapter 3.1 - Installing the Build Scripts

A couple of helper scripts are provided to make build commands less redundant.  To do this, execute the following command:

```
cp [TUX64 BUILD ROOT]/sources/tux64-*/scripts/*.sh [TUX64 BUILD ROOT]/scripts/
```

This will copy the helper scripts to a more convenient location, and also allow us to configure them for our system.  Speaking of which, the `buildconf.sh` script is used to set various global configuration options for the entire Tux64 build.  The following environment variables are provided:

| Name | Description | Default Value |
|------|-------------|---------------|
| TUX64_BUILD_ROOT | The absolute path to [TUX64 BUILD ROOT]. | |
| TUX64_MAKEOPTS | The number of parallel make jobs to run. | $(nproc) |
| TUX64_TARGET_HOST | The target system for the host. | |
| TUX64_TARGET_N64_KERNEL | The target system for the Nintendo 64's bootloader and kernel. | mips64-elf |
| TUX64_TARGET_N64_USERSPACE | The target system for the Nintendo 64's userspace programs. | mips64-linux-musl |
| TUX64_CFLAGS_HOST | Flags to pass to the host's C compiler. | -pipe -march=native -O2 -flto |
| TUX64_CXXFLAGS_HOST | Flags to pass to the host's C++ compiler. | ${TUX64_CFLAGS_HOST} |
| TUX64_ASFLAGS_HOST | Flags to pass to the host's assembler. | |
| TUX64_LDFLAGS_HOST | Flags to pass to the host's linker. | -flto -Wl,--gc-sections -s |
| TUX64_CFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's C compilers. | -pipe -march=vr4300 -mfix4300 -mabi=o64 -Oz -flto -fno-stack-protector -fomit-frame-pointer -ffunction-sections -fdata-sections |
| TUX64_CXXFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's C++ compilers. | ${TUX64_CFLAGS_N64_COMMON} |
| TUX64_ASFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's assemblers. | -march=vr4300 -mtune=vr4300 |
| TUX64_LDFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's linkers. | -flto -Wl,--gc-sections -s |
| TUX64_CFLAGS_N64_KERNEL | Flags to pass to the Nintendo 64's bootloader and kernel C compiler. | ${TUX64_CFLAGS_N64_COMMON} |
| TUX64_CXXFLAGS_N64_KERNEL | Flags to pass to the Nintendo 64's bootloader and kernel C++ compiler. | ${TUX64_CXXFLAGS_N64_COMMON} |
| TUX64_ASFLAGS_N64_KERNEL | Flags to pass to the Nintendo 64's bootloader and kernel assembler. | ${TUX64_ASFLAGS_N64_COMMON} |
| TUX64_LDFLAGS_N64_KERNEL | Flags to pass to the Nintendo 64's bootloader and kernel linker. | ${TUX64_LDFLAGS_N64_COMMON} |
| TUX64_CFLAGS_N64_USERSPACE | Flags to pass to the Nintendo 64's userspace C compiler. | ${TUX64_CFLAGS_N64_COMMON} |
| TUX64_CXXFLAGS_N64_USERSPACE | Flags to pass to the Nintendo 64's userspace C++ compiler. | ${TUX64_CXXFLAGS_N64_COMMON} |
| TUX64_ASFLAGS_N64_USERSPACE | Flags to pass to the Nintendo 64's userspace assembler. | ${TUX64_ASFLAGS_N64_COMMON} |
| TUX64_LDFLAGS_N64_USERSPACE | Flags to pass to the Nintendo 64's userspace linker. | ${TUX64_LDFLAGS_N64_COMMON} |


A couple of these warrant additional explanation.

TUX64_BUILD_ROOT is the absolute path to the build root for Tux64, and must be set manually.  This is required due to issues with relative directories messing up certain `configure` scripts.

TUX64_MAKEOPTS is the number of parallel jobs to run when building software.  This can be thought of as the number of threads to compile software on.  The default is to use `$(nproc)`, which means it will build on all available CPU cores.  This drastically decreases the build time, but also uses significantly more memory and also prevents using other software while building.  If you are having issues running out of memory or want to use other software while building, try manually setting this value to a lower number.

TUX64_TARGET_HOST is the target for the host machine.  This needs to be set manually as there's no easy way to detect this automatically in a simple shell script.  This follows the format `[arch][sub]-[vendor]-[sys]-[env]`.  For example, Linux running glibc on a 64-bit Intel or AMD processor would have the target triple
`x86_64-pc-linux-gnu`.  For more information, refer to [this website](https://clang.llvm.org/docs/CrossCompilation.html).

TUX64_CFLAGS_N64_COMMON includes `-mfix4300`.  This is a flag which patches code to work around hardware bugs in early N64 CPU revisions, at the cost of code size and performance.  We include this flag by default for compatibility with all N64 revisions.  However if you are planning on only running Tux64 on NUS-CPU-04 and later revisions, you should be able to safely remove this flag for improved performance and smaller code size.

After reviewing and setting the required environment variables, these can be exported to the current shell with the following command:

```
. [TUX64 BUILD ROOT]/scripts/buildconf.sh
```

If the shell this command was run in is ever closed, this script will need to be re-run to export the environment variables again.

From this point forward, [TUX64 BUILD ROOT] will be referenced as `${TUX64_BUILD_ROOT}`.

### Chapter 3.2 - Building the Host's Toolchain

Before we can build a cross toolchain, we need to build a host toolchain.  This is so we can build a cross toolchain that was compiled by itself, which gives more consistent behavior, and have no worry about the quality of our system's toolchain.

First, we need to build `binutils`, which can be done with the following:

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-binutils-stage1
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-binutils-stage1

../../sources/binutils-*/configure \
   --disable-dependency-tracking \
   --prefix=${TUX64_BUILD_ROOT}/tools \
   --program-prefix=${TUX64_TARGET_HOST}- \
   CFLAGS="${TUX64_CFLAGS_HOST}" \
   CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
   ASFLAGS="${TUX64_ASFLAGS_HOST}" \
   LDFLAGS="${TUX64_LDFLAGS_HOST}" \
   --enable-host-pie \
   --enable-lto
```

This will create our build directory and configure `binutils`.

Next, we need to build `binutils` with the following:

```
make -j${TUX64_MAKEOPTS}
```

This will compile `binutils`, but not install it to our tools directory.  We can do that with the following:

```
make -j${TUX64_MAKEOPTS} install-strip
```

This will strip the binaries and then install them.  We strip the binaries because there is almost no use for program symbols and unused sections for end-users, and they end up wasting disk space.

We now have our stage-1 `binutils` installed.  Next we configure and build `gcc`.  This process takes 3 stages:

* (stage1) Compile `gcc` with the system's toolchain
* (stage2) Compile `gcc` with the stage1 build
* (stage3) Compile `gcc` with the stage2 build, and compare stage2 and stage3 to make sure they're the same

This achieves a toolchain which was effectively compiled by itself.  The downside is it takes much longer to build, likely hours.  Since it takes so long, let's not waste time and get to building!

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-gcc
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-gcc

../../sources/gcc-*/configure \
   --disable-dependency-tracking \
   --prefix=${TUX64_BUILD_ROOT}/tools \
   CFLAGS="${TUX64_CFLAGS_HOST} -fno-lto" \
   CXXFLAGS="${TUX64_CXXFLAGS_HOST} -fno-lto" \
   ASFLAGS="${TUX64_ASFLAGS_HOST}" \
   LDFLAGS="${TUX64_LDFLAGS_HOST} -fno-lto" \
   --enable-host-pie \
   --enable-lto \
   --enable-bootstrap \
   --enable-languages=c,c++

make -j${TUX64_MAKEOPTS} all
make -j${TUX64_MAKEOPTS} install-strip
```

Once `gcc` finally finishes building and installing, we will have a bootstrapped host toolchain.  Lastly, we need to recompile `binutils` using our fresh host toolchain:

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-binutils-stage2
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-binutils-stage2

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/binutils-*/configure \
      --disable-dependency-tracking \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      --program-prefix=${TUX64_TARGET_HOST}- \
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

Note that the `configure` step also sets environment variables for the toolchain using `usetoolchain.sh`.  Also note that this has to be wrapped using shell slop to prevent the environment variables from leaking to other builds, which could cause major problems.

If successful so far, you now have a fully bootstrapped host toolchain!

### Chapter 3.3 - Building the Bootloader's and Kernel's Toolchain

First we need to build `binutils` for the Nintendo 64's bootloader and kernel toolchain.  Note that we use our bootstrapped host toolchain to build the cross-toolchain, however we are still building code to run on the host right now, so we use the host's compiler flags.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_KERNEL}-binutils
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_KERNEL}-binutils

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/binutils-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_KERNEL} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      --program-prefix=${TUX64_TARGET_N64_KERNEL}- \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}" \
      --enable-host-pie \
      --enable-lto \
      --with-cpu=mips64vr4300
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

Next, we will build `gcc`.  We don't need to build the full toolchain since we will only be using it to build the bootloader and kernel, which only requires the compiler runtime for basic operations.

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_KERNEL}-gcc
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_KERNEL}-gcc

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/gcc-*/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_KERNEL} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      CFLAGS="${TUX64_CFLAGS_HOST} -fno-lto" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST} -fno-lto" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST} -fno-lto" \
      CFLAGS_FOR_TARGET="${TUX64_CFLAGS_N64_KERNEL} -fno-lto" \
      CXXFLAGS_FOR_TARGET="${TUX64_CXXFLAGS_N64_KERNEL} -fno-lto" \
      ASFLAGS_FOR_TARGET="${TUX64_ASFLAGS_N64_KERNEL}" \
      LDFLAGS_FOR_TARGET="${TUX64_LDFLAGS_N64_KERNEL} -fno-lto" \
      --enable-host-pie \
      --enable-lto \
      --disable-bootstrap \
      --enable-languages=c,c++ \
      --with-arch=vr4300 \
      --with-tune=vr4300 \
      --with-abi=o64 \
      --disable-multilib \
      --without-headers \
      --without-shared \
      --disable-libssp
)

make -j${TUX64_MAKEOPTS} all-gcc
make -j${TUX64_MAKEOPTS} install-strip-gcc
make -j${TUX64_MAKEOPTS} all-target-libgcc
make -j${TUX64_MAKEOPTS} install-target-libgcc
```

We now have all the tools required to build the bootloader!

### Chapter 3.4 - Building the Nintendo 64 userspace toolchain

TODO

## Miscellaneous and testing

These are miscellaneous commands being used during development and haven't yet been categorized.

### Building tux64-lib (host)

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-tux64-lib
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_HOST}-tux64-lib

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/tux64-*/lib/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_HOST} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}" \
      --enable-platform-cpu-signed-integer-format-twos-complement \
      --enable-log \
      --enable-log-ansi
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

### Building tux64-lib (bootloader)

```
mkdir ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_KERNEL}-tux64-lib
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_KERNEL}-tux64-lib

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_KERNEL}
   ../../sources/tux64-*/lib/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_KERNEL} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_KERNEL} \
      CFLAGS="${TUX64_CFLAGS_N64_KERNEL}" \
      CXXFLAGS="${TUX64_CXXFLAGS_N64_KERNEL}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_KERNEL}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_KERNEL}" \
      --enable-platform-cpu-signed-integer-format-twos-complement \
      --enable-platform-mips-n64 \
      --enable-platform-mips-vr4300
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

### Building tux64-boot

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-boot
cd ${TUX64_BUILD_ROOT}/builds/tux64-boot

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_KERNEL}
   ../../sources/tux64-*/boot/configure \
      --enable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_N64_KERNEL} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_KERNEL} \
      CFLAGS="${TUX64_CFLAGS_N64_KERNEL}" \
      CXXFLAGS="${TUX64_CXXFLAGS_N64_KERNEL}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_KERNEL}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_KERNEL}"
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

Note that we have to configure with ```--enable-dependency-tracking```.  This is a workaround for an issue with the current version of ```tux64-boot``` where many strange issues pop up if you disable dependency tracking.

### Building tux64-mkrom

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-mkrom
cd ${TUX64_BUILD_ROOT}/builds/tux64-mkrom

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_HOST}
   ../../sources/tux64-*/mkrom/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_HOST} \
      --target=${TUX64_TARGET_HOST} \
      --prefix=${TUX64_BUILD_ROOT}/tools \
      CFLAGS="${TUX64_CFLAGS_HOST}" \
      CXXFLAGS="${TUX64_CXXFLAGS_HOST}" \
      ASFLAGS="${TUX64_ASFLAGS_HOST}" \
      LDFLAGS="${TUX64_LDFLAGS_HOST}"
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

### Using tux64-mkrom to create a ROM image

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-rom-image
cd ${TUX64_BUILD_ROOT}/builds/tux64-rom-image
```

Create a file named ```${TUX64_BUILD_ROOT}/builds/tux64-rom-image/mkrom-config``` with the following contents:

```
# tux64-mkrom sample configuration

--bootloader-stage0
/share/tux64-boot/stage0.bin

--bootloader-stage0-cic
/share/tux64-boot/stage0.bin.cic

--bootloader-stage1
/share/tux64-boot/stage1.bin

--bootloader-stage2
/share/tux64-boot/stage2.bin

--kernel
/boot/vmlinuz

--initramfs
/boot/initramfs.cpio
```

Create a ROM image with the following:

```
${TUX64_BUILD_ROOT}/tools/bin/tux64-mkrom -c mkrom-config -o tux64.n64 -p ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_KERNEL}
```