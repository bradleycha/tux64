# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 1 - Setup](01-setup.md)
* Next Page: [Chapter 3 - Building The Toolchains](03-building-the-toolchains.md)

## Chapter 2 - Obtaining The Sources

Before we can start building the required packages, we need to download their
sources (short for source-code tarballs, or just source-code).  Many of the
packages also offer signatures.  **It is highly recommended to verify signatures
when possible, otherwise you may run malware on your computer.**  Even though we
are building software to run on the Nintendo 64, we are still executing builds
_scripts_, which execute code on our host machine.  If we don't verify
signatures, this can leave us open to hackers providing malicious sources
bundled with malware in the build scripts.

### Chapter 2.1 - Verifying Authenticity of Sources

To verify signatures for a downloaded package, you will need GNU Privacy Guard,
otherwise known as GPG.  Once you have GPG, you will need three components:  The
package sources, the signature for the package sources, and the signer's public
key.

The signature for the package sources are often bundled right next to the
sources themselves, but sometimes it may be a little tricky to find.  Some
software doesn't provide digital signatures at all, instead relying on HTTPS or
other forms on protected transmission to provide data integrity.

The signer's public key can be a little more tricky to locate.  Often, either
the project's website will offer their public key, or the individual signer for
the release will have a personal site which offers their public key.  These can
be tricky to find, but it's worth protecting yourself against hackers when
possible.  You can also attempt to search for public keys from a key server with
the following:

```
gpg --locate-keys [QUERY]
```

For example, to find public keys for Linux maintainers, you would type the
following:

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

Using the above example again, if we wanted to import Greg Kroah-Hartman's
public key, we would type the following:

```
gpg --recv-keys 647F28654894E3BD457199BE38DBBDC86092693E
```

### Chapter 2.2 - Required Packages

Provided is a table of all the required sources, and their recommended version.
While it is likely possible to use packages with different versions or external
patches, we don't guarantee compatibility with the rest of the packages, and
will likely require extra work to make it work.

Every package listed should be stored uncompressed inside of `sources/`, with
the format `[NAME]-[VERSION]`.

| Package | Version | Notes |
|---------|---------|-------|
| [tux64](https://github.com/bradleycha/tux64/) | master | Currently no stable release.  Use `git` to clone the latest version of the 'master' branch.  If cloning directly from GitHub, you must run ```autoreconf -i```  for each sub-project which contains a GNU Autoconf script (```configure.ac```) |
| [binutils](https://www.gnu.org/software/binutils/) | 2.46.1 | |
| [gcc](https://gcc.gnu.org/) | 16.1.0 | |
| [musl](https://musl.libc.org/) | 1.2.6 | |
| [linux](https://kernel.org/) | 6.18.35 | Signatures should be checked against the uncompressed tarball, not the compressed one (i.e. `xz --decompress linux-*.tar.xz && gpg --verify linux-*.tar.sign`). |

### Chapter 2.3 - Installing Scripts

A couple of helper scripts are provided to make build commands less redundant.
To do this, execute the following command:

```
cp [TUX64 BUILD ROOT]/sources/tux64-*/scripts/*.sh [TUX64 BUILD ROOT]/scripts/
```

This will copy the helper scripts to a more convenient location, and also allow
us to configure them for our system.  Speaking of which, the `buildconf.sh`
script is used to set various global configuration options for the entire Tux64
build.  The following environment variables are provided:

| Name | Description | Default Value |
|------|-------------|---------------|
| TUX64_BUILD_ROOT | The absolute path to \[TUX64 BUILD ROOT\]. | |
| TUX64_MAKEOPTS | The number of parallel make jobs to run. | $(nproc) |
| TUX64_TARGET_HOST | The target system for the host. | |
| TUX64_TARGET_N64_BOOTLOADER | The target system for the Nintendo 64's bootloader. | mips64-elf |
| TUX64_TARGET_N64_LINUX | The target system for the Nintendo 64's kernel and userspace programs. | mips-linux-musl |
| TUX64_CFLAGS_HOST | Flags to pass to the host's C compiler. | -pipe -march=native -O2 -flto |
| TUX64_CXXFLAGS_HOST | Flags to pass to the host's C++ compiler. | ${TUX64_CFLAGS_HOST} |
| TUX64_ASFLAGS_HOST | Flags to pass to the host's assembler. | |
| TUX64_LDFLAGS_HOST | Flags to pass to the host's linker. | -Wl,--gc-sections -flto |
| TUX64_CFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's C compilers. | -pipe -march=vr4300 -mfix4300 -Oz -flto -fno-stack-protector |
| TUX64_CXXFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's C++ compilers. | ${TUX64_CFLAGS_N64_COMMON} |
| TUX64_ASFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's assemblers. | -march=vr4300 -mtune=vr4300 |
| TUX64_LDFLAGS_N64_COMMON | Shared flags to pass to the Nintendo 64's linkers. | -Wl,--gc-sections -flto |
| TUX64_CFLAGS_N64_BOOTLOADER | Flags to pass to the Nintendo 64's bootloader C compiler. | ${TUX64_CFLAGS_N64_COMMON} -mabi=o64 -G65536 -mexplicit-relocs=none -mno-check-zero-division |
| TUX64_ASFLAGS_N64_BOOTLOADER | Flags to pass to the Nintendo 64's bootloader assembler. | ${TUX64_ASFLAGS_N64_COMMON} |
| TUX64_LDFLAGS_N64_BOOTLOADER | Flags to pass to the Nintendo 64's bootloader linker. | ${TUX64_LDFLAGS_N64_COMMON} |
| TUX64_CFLAGS_N64_KERNEL | Flags to pass to the Nintendo 64's kernel C compiler. | ${TUX64_CFLAGS_N64_COMMON} -fno-lto |
| TUX64_ASFLAGS_N64_KERNEL | Flags to pass to the Nintendo 64's kernel assembler. | ${TUX64_ASFLAGS_N64_COMMON} |
| TUX64_CFLAGS_N64_LINUX | Flags to pass to the Nintendo 64's userspace C compiler. | ${TUX64_CFLAGS_N64_COMMON} -mabi=32 |
| TUX64_CXXFLAGS_N64_LINUX | Flags to pass to the Nintendo 64's userspace C++ compiler. | ${TUX64_CFLAGS_N64_LINUX} |
| TUX64_ASFLAGS_N64_LINUX | Flags to pass to the Nintendo 64's userspace assembler. | ${TUX64_ASFLAGS_N64_COMMON} |
| TUX64_LDFLAGS_N64_LINUX | Flags to pass to the Nintendo 64's userspace linker. | ${TUX64_LDFLAGS_N64_COMMON} |

A couple of these warrant additional explanation.

`${TUX64_BUILD_ROOT}` is the absolute path to the build root for Tux64, and must
be set manually.  This is required due to issues with relative directories
messing up certain `configure` scripts.

`${TUX64_MAKEOPTS}` is the number of parallel jobs to run when building
software.  This can be thought of as the number of threads to compile software
on.  The default is to use `$(nproc)`, which means it will build on all
available CPU cores.  This drastically decreases the build time, but also uses
significantly more memory and also prevents using other software while building.
If you are having issues running out of memory or want to use other software
while building, try manually setting this value to a lower number.

`${TUX64_TARGET_HOST}` is the target for the host machine.  This needs to be set
manually as there's no easy way to detect this automatically in a simple shell
script.  This follows the format `[arch][sub]-[vendor]-[sys]-[env]`.  For
example, Linux running glibc on a 64-bit Intel or AMD processor would have the
target triple `x86_64-pc-linux-gnu`.  For more information, refer to
[this website](https://clang.llvm.org/docs/CrossCompilation.html).

`${TUX64_TARGET_N64_BOOTLOADER}` is set to target 64-bit MIPS to make use of
GCC's O64 ABI, which provides 32-bit addresses with 64-bit registers.  As the
VR4300's virtual address space is only 32-bit, there is not purpose to 64-bit
pointers.  In the future, the kernel and userspace will also be built with
`-mabi=o64`, but currently it's unsupported outside of the bootloader.

`${TUX64_TARGET_N64_LINUX}` is set to use 32-bit MIPS because 32-bit code is
smaller on-average than 64-bit code.  Also, 64-bit kernels are currently broken
and crash when attempting to run any userspace programs.  If you want, you are
welcome to try setting this to `mips64-linux-musl` instead to see what happens.
Just make sure to update `TUX64_CFLAGS_N64_LINUX` to use `-mabi=64`, and select
`64-bit kernel` for the kernel configuration.

`${TUX64_LDFLAGS_HOST}` and `${TUX64_LDFLAGS_N64_COMMON}` include
`-Wl,--gc-sections`.  This is a flag which removes unused sections when linking
programs.  This can help with dead code elimination, thus producing smaller
binaries.

`${TUX64_CFLAGS_N64_COMMON}` includes `-mfix4300`.  This is a flag which patches
code to work around hardware bugs in early N64 CPU revisions, at the cost of
code size and performance.  We include this flag by default for compatibility
with all N64 revisions.  However if you are planning on only running Tux64 on
NUS-CPU-04 and later revisions, you should be able to safely remove this flag
for improved performance and smaller code size.

`${TUX64_CFLAGS_N64_BOOTLOADER}` includes `-mabi=o64`.  This uses GCC's MIPS O64
ABI, as documented above.  A few other CFLAGS are also included which make the
bootloader smaller, but are generally unsafe to use outside of the bootloader.

After reviewing and setting the required environment variables, these can be
exported to the current shell with the following command:

```
. [TUX64 BUILD ROOT]/scripts/buildconf.sh
```

If the shell in which this command was run in is ever closed, the above command
will need to be re-run to export the environment variables again.

From this point forward, \[TUX64 BUILD ROOT\] will be referenced as the shell
variable `${TUX64_BUILD_ROOT}`.

### Chapter 2.4 - Apply Source Patches

Some packages have patches which are applied to upstream sources.  This is done
so complete package sources do not have to be distributed seperately, as well as
make the code changes forward-compatible with future package versions.

To apply patches to required packages, run the following command:

```
for pkg in gcc linux; do
    pushd ${TUX64_BUILD_ROOT}/sources/$pkg-*
    git apply ${TUX64_BUILD_ROOT}/sources/tux64-*/patches/$pkg-*.patch
    popd
done
```

We will now proceed to [building the toolchains](03-building-the-toolchains.md).

