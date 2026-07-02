# Tux64 Installation Guide

* Next Page: [Chapter 1 - Setup](01-setup.md)

## Chapter 0 - Introduction

Welcome to Tux64!  This distribution of Linux aims to bring a minimal, but
hopefully usable terminal Linux environment to the Nintendo 64.  This guide
will start with source code for individual packages and end with a bootable ROM
image which contains the entire operating system.

This is a long and tedious process, involving bootstrapping multiple compiler
toolchains from source, configuring a Linux kernel, and building many other
userspace applications.  While this guide aims to make this process as painless
as possible, it's still a difficult process intended for experienced Linux
users.  For reference, this guide is about the same level as Linux from Scratch.
It is recommended to be comfortable with installing Gentoo Linux, as much of the
general process of installing Gentoo Linux will be seen here, minus the package
manager and "stage" files.  Additionally, having experience with C or C++
toolchains and build systems, such a GCC and Autotools, may prove invaluable,
especially if you encounter issues during the build process.

### Chapter 0.1 - Prerequisites

We will be building most of our software using GNU Autotools, GNU Make, and
other free software.  If building on Windows, it's recommended to use Windows
Subsystem for Linux and perform compilation within there.  If on Linux or some
flavor of BSD, you likely already have all the necessary tools to build Tux64.
If not, you may have to download your distribution's "developer" toolchain.

For example, on Debian, Ubuntu, Mint, etc., you would type the following:

```
apt-get install build-essential
```

Or if you're on Arch Linux or any of its derivatives, you would type the
following:

```
pacman -S base-devel
```

Additionally, if you would like to run your Tux64 build, you will need a way to
run Nintendo 64 ROMs.  You could either choose to use an emulator, or run it on
real hardware.  For real hardware, you will need a flashcart which can run ROMs
from a file, such as an EverDrive64 or SummerCart64.  For an emulator, you will
need a modern LLE emulator.  This excludes emulators such as Project64 and
Mupen64(Plus), as these are designed to run commercial games with high
performance, at the cost of emulation accuracy.  I recommend either
[Ares](https://ares-emu.net/) or [dgb-n64](https://github.com/Dillonb/n64).

We will now proceed to [setting up the build root](01-setup.md).

