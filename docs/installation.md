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
| [binutils](https://www.gnu.org/software/binutils/) | 2.45 | |
| [gcc](https://gcc.gnu.org/) | 15.2.0 | |
| [musl](https://musl.libc.org/) | 1.2.5 | |
| [linux](https://kernel.org/) | 6.12.57 | Signatures should be checked against the uncompressed tarball, not the compressed one (i.e. `xz --decompress linux-*.tar.xz && gpg --verify linux-*.tar.sign`). |

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
| TUX64_TARGET_N64_BOOTLOADER | The target system for the Nintendo 64's bootloader. | mips64-elf |
| TUX64_TARGET_N64_LINUX | The target system for the Nintendo 64's kernel and userspace programs. | mips64-linux-musl |
| TUX64_CFLAGS_HOST | Flags to pass to the host's C compiler. | -pipe -march=native -O2 -flto |
| TUX64_CXXFLAGS_HOST | Flags to pass to the host's C++ compiler. | ${TUX64_CFLAGS_HOST} |
| TUX64_ASFLAGS_HOST | Flags to pass to the host's assembler. | |
| TUX64_LDFLAGS_HOST | Flags to pass to the host's linker. | -flto |
| TUX64_CFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's C compilers. | -pipe -march=vr4300 -mfix4300 -mabi=64 -Oz -flto -fno-stack-protector |
| TUX64_CXXFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's C++ compilers. | ${TUX64_CFLAGS_N64_COMMON} |
| TUX64_ASFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's assemblers. | -march=vr4300 -mtune=vr4300 |
| TUX64_LDFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's linkers. | -flto |
| TUX64_CFLAGS_N64_BOOTLOADER | Flags to pass to the Nintendo 64's bootloader C compiler. | ${TUX64_CFLAGS_N64_COMMON} -mabi=o64 |
| TUX64_ASFLAGS_N64_BOOTLOADER | Flags to pass to the Nintendo 64's bootloader assembler. | ${TUX64_ASFLAGS_N64_COMMON} |
| TUX64_LDFLAGS_N64_BOOTLOADER | Flags to pass to the Nintendo 64's bootloader linker. | ${TUX64_LDFLAGS_N64_COMMON} |
| TUX64_CFLAGS_N64_KERNEL | Flags to pass to the Nintendo 64's kernel C compiler. | ${TUX64_CFLAGS_N64_COMMON} -fno-lto |
| TUX64_ASFLAGS_N64_KERNEL | Flags to pass to the Nintendo 64's kernel assembler. | ${TUX64_ASFLAGS_N64_COMMON} |
| TUX64_CFLAGS_N64_LINUX | Flags to pass to the Nintendo 64's userspace C compiler. | ${TUX64_CFLAGS_N64_COMMON} |
| TUX64_CXXFLAGS_N64_LINUX | Flags to pass to the Nintendo 64's userspace C++ compiler. | ${TUX64_CXXFLAGS_N64_COMMON} |
| TUX64_ASFLAGS_N64_LINUX | Flags to pass to the Nintendo 64's userspace assembler. | ${TUX64_ASFLAGS_N64_COMMON} |
| TUX64_LDFLAGS_N64_LINUX | Flags to pass to the Nintendo 64's userspace linker. | ${TUX64_LDFLAGS_N64_COMMON} |

A couple of these warrant additional explanation.

TUX64_BUILD_ROOT is the absolute path to the build root for Tux64, and must be set manually.  This is required due to issues with relative directories messing up certain `configure` scripts.

TUX64_MAKEOPTS is the number of parallel jobs to run when building software.  This can be thought of as the number of threads to compile software on.  The default is to use `$(nproc)`, which means it will build on all available CPU cores.  This drastically decreases the build time, but also uses significantly more memory and also prevents using other software while building.  If you are having issues running out of memory or want to use other software while building, try manually setting this value to a lower number.

TUX64_TARGET_HOST is the target for the host machine.  This needs to be set manually as there's no easy way to detect this automatically in a simple shell script.  This follows the format `[arch][sub]-[vendor]-[sys]-[env]`.  For example, Linux running glibc on a 64-bit Intel or AMD processor would have the target triple
`x86_64-pc-linux-gnu`.  For more information, refer to [this website](https://clang.llvm.org/docs/CrossCompilation.html).

TUX64_CFLAGS_N64_COMMON includes `-mfix4300`.  This is a flag which patches code to work around hardware bugs in early N64 CPU revisions, at the cost of code size and performance.  We include this flag by default for compatibility with all N64 revisions.  However if you are planning on only running Tux64 on NUS-CPU-04 and later revisions, you should be able to safely remove this flag for improved performance and smaller code size.

TUX64_CFLAGS_N64_BOOTLOADER includes `-mabi=o64`.  This uses GCC's MIPS O64 ABI, which is basically the regular 64-bit ABI but with 32-bit pointers.  Since the VR4300's virtual address space is only 32-bit, there's no purpose to 64-bit pointers.  In the future, the kernel and userspace will also be built with `-mabi=o64`, but currently it's unsupported outside of the bootloader.

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
   --host=${TUX64_TARGET_HOST} \
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
   --host=${TUX64_TARGET_HOST} \
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
      --host=${TUX64_TARGET_HOST} \
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

### Chapter 3.3 - Building the Bootloader's Toolchain

First we need to build `binutils` for the Nintendo 64's bootloader toolchain.  Note that we use our bootstrapped host toolchain to build the cross-toolchain, however we are still building code to run on the host right now, so we use the host's compiler flags.

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
      --enable-host-pie \
      --enable-lto \
      --with-cpu=mips64vr4300
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

Next, we will build `gcc`.  We don't need to build the full toolchain since we will only be using it to build the bootloader, which only requires the compiler runtime for basic operations.

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

### Chapter 3.4 - Building the Nintendo 64 kernel and userspace toolchain

Now we need to build the toolchail which will compile the Nintendo 64's Linux kernel and userspace software.  This is the most tedious part, because we have to build `musl` to build the toolchain, but we need the toolchain to build `musl`.

First we build `binutils` as we did for the previous parts, but now targetting Linux instead of the bootloader.

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
      --enable-host-pie \
      --enable-lto \
      --with-cpu=mips64vr4300
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

Next, we build the stage-1 `gcc`.  This will be used to compile `musl`, which whill then be used to build the full `gcc` toolchain.  We only want to build and install the compiler itself, as `libgcc` depends on `musl`.

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

Once this completes, we need to configure `musl` and install its headers and compiler runtime.  We will build a shared object for `musl` once our toolchain is complete.

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

Now we will return to the stage-1 `gcc` build to build the rest of the toolchain.

```
cd ${TUX64_BUILD_ROOT}/builds/${TUX64_TARGET_N64_LINUX}-gcc-stage1
make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install-strip
```

We now have our stage-1 toolchain.  We will use this to build the full-featured stage-2 toolchain.

First, we will install kernel headers.  These are required for various supporting `gcc` libraries.

```
mkdir ${TUX64_BUILD_ROOT}/builds/linux-headers
cd ${TUX64_BUILD_ROOT}/builds/linux-headers

${TUX64_BUILD_ROOT}/scripts/kernel-make.sh headers_install
```

TODO: Build full stage-2 `gcc` and `musl`.  We want to build both static and shared objects and also make use of LTO.  We also want to build the libraries above we disabled.

## Chapter 4 - Building userspace software

Before we continue, we need to build various tools and libraries which will be used later in the guide.

### Chapter 4.1 - Building tux64-lib

`tux64-lib` is a library for Tux64 programs which contains globally-shared functionality.  This is required for all platforms which will run Tux64-specific programs.

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

### Chapter 4.2 - Building tux64-mkrom

`tux64-mkrom` is a tool used to create the final bootable ROM image which will run on the Nintendo 64.  This will be needed near the end of the installation.

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

## Chapter 5 - Building the Linux kernel

Now is the time you've been waiting for!  Time to build a Linux kernel for the Nintendo 64!

Since the kernel is built with raw Makefiles, there is no `configure` step to store the build tools and flags.  This means we have to specify our toolchain every time we run a make command.  To mitigate this, we'll make use of the `kernel-make` script.  If you'd like to customize your kernel make command, take a look at `${TUX64_BUILD_ROOT}/scripts/kernel-make.sh`.

### Chapter 5.1 - Configuring the kernel

First, we'll set up our build directory and generate a minimal config.

```
mkdir ${TUX64_BUILD_ROOT}/builds/linux
cd ${TUX64_BUILD_ROOT}/builds/linux

${TUX64_BUILD_ROOT}/scripts/kernel-make.sh tinyconfig
```

Next, we'll launch the configuration tool and begin configuring the kernel.

```
${TUX64_BUILD_ROOT}/scripts/kernel-make.sh menuconfig
```

We will need to set the following configuration items:

TODO: verify these actually boot.  these are here for testing and almost definitely don't work.

```
Machine selection  --->
   System type  --->
      (X) Nintendo 64 console

Kernel type  --->
   Kernel code model  --->
      (X) 64-bit kernel

General setup  --->
   Local version - append to kernel release
      -tux64
```

Note that you don't necessarily have to use "-tux64" as the local version, but the branding is nice. :)

Make sure to save the kernel config before exiting.  You are now ready to build a kernel image.

### Chapter 5.2 - Building the kernel

Building the kernel should be as simple as a single `make` command:

```
${TUX64_BUILD_ROOT}/scripts/kernel-make.sh
```

The kernel image will be named `vmlinuz` in the build directory.

### Chapter 5.3 - Installing the kernel

Now we need to install the kernel image and modules.  The kernel image will be used by the bootloader, while the modules will be used once we are already booted into linux.

We first install the kernel image to the bootloader tools directory with the following:

```
mkdir ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot
cp vmlinuz ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/
```

We then install kernel modules to the Nintendo 64's root filesystem with the following:

```
${TUX64_BUILD_ROOT}/scripts/kernel-make.sh modules_install
```

We now have a fully built Linux kernel!

### Chapter 5.4 - Creating the initramfs

TODO: figure out what exactly we need in our initramfs, if we even need one at all.  for now, just do the following:

```
touch ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/initramfs.cpio
```

## Chapter 6 - Creating a bootable ROM image

We now have a kernel image and a bunch of software, but how do we boot this?  With a bootloader, of course!  Specifically, `tux64-boot`.

### Chapter 6.1 - Building tux64-boot

We first need to build the bootloader.  This will produce a series of binary "stage" files.  These are the executable code which will run right after the console powers on and starts executing code from the cartridge.

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-boot
cd ${TUX64_BUILD_ROOT}/builds/tux64-boot

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_BOOTLOADER}
   ../../sources/tux64-*/boot/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_BOOTLOADER} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER} \
      CFLAGS="${TUX64_CFLAGS_N64_BOOTLOADER}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_BOOTLOADER}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_BOOTLOADER}"
)

make -j1
make -j1 install
```

Notice how we only build with a single job.  This is because with the current version of `tux64-boot`, multithreaded builds are broken and result in strange behavior.

### Chapter 6.2 - Configuring the ROM image

We will be creating a bootable ROM image using `tux64-mkrom`.  This tool has various options to confiure the output ROM, all of which can be seen by running `tux64-mkrom --help`.

We'll first create our directory for the configuration file and final ROM image:

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-rom-image
cd ${TUX64_BUILD_ROOT}/builds/tux64-rom-image
touch mkrom-config
```

If you'd like a nice sample configuration, the following should work for most people:

```
# tux64-mkrom sample configuration

--bootloader-stage0
/share/tux64-boot/stage0.bin

--bootloader-stage0-cic
/share/tux64-boot/stage0.bin.cic

--bootloader-stage1
/share/tux64-boot/stage1.bin

--bootloader-stage1-bss
/share/tux64-boot/stage1.bin.bss

--bootloader-stage2
/share/tux64-boot/stage2.bin

--kernel
/boot/vmlinuz

--initramfs
/boot/initramfs.cpio
```

### Chapter 6.3 - Building the ROM image

We now have all the pieces to build the ROM image.  Build it with the following:

```
${TUX64_BUILD_ROOT}/tools/bin/tux64-mkrom \
   -p ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER} \
   -c mkrom-config \
   -o tux64.n64
```

Our final Tux64 ROM image should now be output to `tux64.n64`!
