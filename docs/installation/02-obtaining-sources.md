# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 1 - Setup](01-setup.md)
* Next Page: [Chapter 3 - Building The Toolchains](03-building-the-toolchains.md)

## Chapter 2 - Obtaining Sources

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
| [binutils](https://www.gnu.org/software/binutils/) | 2.47 | |
| [gcc](https://gcc.gnu.org/) | 16.2.0 | |
| [e2fsprogs](https://e2fsprogs.sourceforge.net/) | 1.47.4 | Signatures should be checked against the uncompressed tarball, not the compressed one (i.e. `xz --decompress e2fsprogs-*.tar.xz && gpg --verify e2fsprogs-*.tar.sign`). |
| [linux](https://kernel.org/) | 6.18.54 | Same note as for `e2fsprogs`. |
| [musl](https://musl.libc.org/) | 1.2.6 | It is highly recommended to apply the security patches listed on the homepage.  You can download the patch file (link is the underlined "patch"/"patched") and apply it with `cd musl-* && git apply [patch]`. |
| [zlib](https://zlib.net/) | 1.3.2 | |
| [openssl](https://openssl-library.org/) | 4.0.3 | |
| [gmp](https://gmplib.org/) | 6.3.0 | |
| [coreutils](https://www.gnu.org/software/coreutils/) | 9.12 | |
| [bash](https://www.gnu.org/software/bash/) | 5.3 | |

### Chapter 2.3 - Installing Scripts

A couple of helper scripts are provided to make build commands less redundant.
To do this, execute the following command:

```
cp [TUX64 BUILD ROOT]/sources/tux64-*/scripts/*.sh [TUX64 BUILD ROOT]/scripts/
```

This will copy the helper scripts to a more convenient location, and also allow
us to configure them for our system.  Speaking of which, the `buildconf.sh`
script is used to set various global configuration options for the entire Tux64
build.  The variables should be self-explanatory, but there are some which
warrant additional explanation.

`${TUX64_BUILD_ROOT}` is the absolute path to the build root.  This must be
specified manually.

`${TUX64_TARGET_HOST}` is the target triple of the host.  For example, for a
64-bit Intel or AMD computer running Linux with Glibc, the target triple would
be `x86_64-pc-linux-gnu`.  This needs to be specified manually.  If confused,
running `gcc -dumpmachine` usually provides the correct target triple.  For more
information, click [here](https://wiki.osdev.org/Target_Triplet).

`${TUX64_MAKEOPTS}` specifies the number of parallel build jobs to run when
compiling software with `make`.  By default, this is set to `$(nproc)`, which
uses all available threads (nproc = Number of Processors, how about that!?).
Since this can run multiple jobs at once, memory usage will increase
proportionally with the number of parallel jobs.  If you are having memory
issues, you may want to manually reduce this.  Additionally, you may want to
reduce this if you would like to not pin your CPU at 100% for hours at a time.

`${TUX64_CFLAGS_COMMON}` sets C compiler flags which are used for all build
targets.  In particular, `-pipe` and `-flto` are used.  `-pipe` uses in-memory
pipes instead of the filesystem to handle passing of temporary files between
stages of compilation.  This speeds up compilation, but and uses more memory.
If you are having issues with running out of memory, you may want to remove
`-pipe`.  Additionally, `-flto` enables Link-Time Optimization (LTO).  LTO
allows optimization of an entire program, producing more efficient code.
However, LTO tends to increase memory usage and compilation time.  If either of
these are problems, you may want to remove this flag.

`${TUX64_CFLAGS_N64_LINUX}` sets the C compiler flags for all userspace software
running under Linux on the Nintendo 64.  Notice that `-mabi=32` is specified.
32-bit code tends to be smaller on-average to 64-bit code, therefore 32-bit code
is generated by default.  If you would like to use 64-bit code, you must also
update the kernel configuration to support 64-bit binaries.

After reviewing and setting the given environment variables, these can be
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

