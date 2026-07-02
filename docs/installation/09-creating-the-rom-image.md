# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 8 - Building The Bootloader](08-building-the-bootloader.md)
* Next Page: [Chapter 10 - Installing The ROM Image and Root Filesystem](10-installing-the-rom-image-and-root-filesystem.md)

## Chapter 9 - Creating The ROM Image

We are now ready to create a bootable ROM image which contains all of the
necessary components to boot Linux on the Nintendo 64.  This is the file we will
be loading either into an emulator or onto a flashcart to boot Linux.

### Chapter 9.1 - Configuring The ROM Image

We will be creating a bootable ROM image using `tux64-mkrom`.  This tool has
various options to confiure the output ROM, all of which can be seen by running
`tux64-mkrom --help`.

We'll first create our directory for the configuration file and final ROM image:

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-rom-image
cd ${TUX64_BUILD_ROOT}/builds/tux64-rom-image
touch mkrom-config
```

If you'd like a nice sample configuration, the following should work for most
people:

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
/boot/vmlinux

--initramfs
/boot/initramfs.cpio

--memory-display
```

### Chapter 9.2 - Building The ROM Image

We now have all the pieces to build the ROM image.  Build it with the following:

```
${TUX64_BUILD_ROOT}/tools/bin/tux64-mkrom \
   -p ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER} \
   -c mkrom-config \
   -o tux64.n64
```

Our final Tux64 ROM image should now be output to `tux64.n64`!

We will now proceed to [installing the ROM image and root filesystem](10-installing-the-rom-image-and-root-filesystem.md).

