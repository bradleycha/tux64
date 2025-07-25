# Tux64
A minimal, yet functional, Linux distribution for the Nintendo 64.

## Setting Expectations
The goal of this project is to provide a usable Linux terminal environment for
the Nintendo 64.  The goal is to provide functionality for both a stock
console, as well as interfaces for flashcarts, such as the Everdrive64 and
SummerCart64, which may allow connecting external devices, such as a USB
keyboard, to the Nintendo 64.

This is not a port of Desktop Linux to the Nintendo 64.  Due to the Nintendo
64's ancient (albeit revolutionary) graphics architecture, it is unlikely 3D
acceleration will ever be possible.  At most, a custom graphics driver with a
fully custom userspace API will be written, which will require porting existing
applications to.

## Recommended Experience
While the installation guide is designed to be as user-friendly and approachable
as possible, this is an advanced guide which requires bootstrapping multiple
compiler toolchains from source, configuring a Linux kernel, and configuring and
compiling the entire suite of userspace software from source.  The level of
difficulty for installing Tux64 is on the same level as Linux from Scratch.  If
you can install Gentoo Linux, then you will have a great foundation for this
project.  It is also very helpful to have a basic knowledge of C or C++
programming and using compilers such as GCC or Clang, as well as experience with
GNU Make and Autotools.  If you run into undocumented issues while installing,
this knowledge will prove to be invaluable.

## Hey, isn't there already [Linux on the Nintendo 64?](https://www.github.com/clbr/n64bootloader)
Indeed, Linux has already been ported to the Nintendo 64 by a talented hacker.
However, this only provides an extremely barebones bootloader and kernel patches
for a now over 4-year-old LTS kernel that will soon reach EOL.  In addition, the
build system is terrible, and there's almost zero documentation on how to
actually build the project into a bootable ROM image.  This project will provide
documentation and custom software to fix these issues, while building upon the
original with an improved bootloader, up-to-date kernel patches, and more.

## Getting Started
To get started, read the [Installation Guide](https://github.com/bradleycha/tux64/wiki/Installation-Guide).

## Credits
Special thanks to the following projects.  Without these, this project would be
significantly more difficult, if not impossible:
 - [Ares](https://ares-emu.net) for providing a tolerable development environment for the bootloader
 - [ipl3hasher-new](https://github.com/Polprzewodnikowy/ipl3hasher-new) for allowing `tux64-boot` to boot freestanding on real hardware
 - [Libdragon](https://libdragon.dev) for providing examples of low-level software implementation details
 - [N64brew Wiki](https://n64brew.dev) for providing detailed hardware information about the N64
 - And users like you.  Thank you.

