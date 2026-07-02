# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 4 - Building Userspace Software](04-building-userspace-software.md)
* Next Page: [Chapter 6 - Creating The Initramfs](06-creating-the-initramfs.md)

## Chapter 5 - Building The Kernel

Now is the time you've been waiting for!  Time to build a Linux kernel for the
Nintendo 64!

Since the kernel is built with raw Makefiles, there is no `configure` step to
store the build tools and flags.  This means we have to specify our toolchain
every time we run a make command.  To mitigate this, we'll make use of the
`kernel-make` script.  If you'd like to customize your kernel make command, take
a look at `${TUX64_BUILD_ROOT}/scripts/kernel-make.sh`.

### Chapter 5.1 - Configuring The Kernel

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

```
Machine selection  --->
  System type  --->
    (X) Nintendo 64 console

Kernel type  --->
  Kernel code model  --->
    (X) 32-bit kernel

  [*] Floating Point support
  [*] Tux64 support

General setup  --->
  Local version - append to kernel release
    -tux64

  [*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
  [*] Support initial ramdisk/ramfs compressed using gzip
  [ ] Support initial ramdisk/ramfs compressed using bzip2
  [ ] Support initial ramdisk/ramfs compressed using LZMA
  [ ] Support initial ramdisk/ramfs compressed using XZ
  [ ] Support initial ramdisk/ramfs compressed using LZO
  [ ] Support initial ramdisk/ramfs compressed using LZ4
  [ ] Support initial ramdisk/ramfs compressed using ZSTD

  Configure standard kernel features (expert users)  --->
    [*] Enable support for printk

Executable file formats  --->
  [*] Kernel support for ELF binaries
  [*] Kernel support for scripts starting with #!

Device drivers  --->
  Character devices  --->
    [*] Enable TTY
    [*]   Virtual terminal
    [ ]     Enable character translations in console
    [*]     Support for console on virtual terminal
    [ ]   Unix98 PTY support
    [ ]   Legacy (BSD) PTY support
    [ ]   Allow legacy TIOCSTI usage
    [ ]   Automatically load TTY Line Disciplines

  Graphics support  --->
    Frame buffer Devices  --->
      [*] Support for frame buffer device drivers
      [*] Simple framebuffer support
      [ ] Provide legacy /dev/fb* device

    Console display driver support  --->
      [*] Framebuffer Console support
      [*] Map the console to the primary display device

    [*] Bootup logo  --->
      [ ] Standard black and white Linux logo
      [ ] Standard 16-color Linux logo
      [*] Standard 224-color Linux logo

  Input device support  --->
    [ ]   Keyboards  ----
    [ ]   Mice  ----

    Hardware I/O ports  --->
      [ ] Serial I/O support

  [ ] Device Tree and Open Firmware support
  [ ] HID bus support

Kernel hacking  --->
  printk and dmesg options  --->
    [*] Show timing information on printk
```

Note that you don't necessarily have to use "-tux64" as the local version, but
the branding is nice. :)

Make sure to save the kernel config before exiting.  You are now ready to build
a kernel image.

### Chapter 5.2 - Building The Kernel

Building the kernel should be as simple as a single `make` command:

```
${TUX64_BUILD_ROOT}/scripts/kernel-make.sh
```

The kernel image will be named `vmlinux` in the build directory.  Note that
there is also a `vmlinuz`, with a `z`.  This is a compressed kernel image which
decompresses itself at runtime.  While this may sound good, the decompression
code uses a (relatively) huge amount of memory, and is not recommended to use
over the standard kernel image.

### Chapter 5.3 - Installing The Kernel

Now we need to install the kernel image and modules.  The kernel image will be
used by the bootloader, while the modules will be used once we are already
booted into linux.

We first install the kernel image to the bootloader tools directory with the
following:

```
mkdir ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot
cp vmlinux ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/
```

We then install kernel modules to the Nintendo 64's root filesystem with the
following:

```
${TUX64_BUILD_ROOT}/scripts/kernel-make.sh modules_install
```

We will now proceed to [creating the initramfs](06-creating-the-initramfs.md).

