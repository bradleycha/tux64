# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 6 - Creating The Initramfs](06-creating-the-initramfs.md)
* Next Page: [Chapter 8 - Building The Bootloader](08-building-the-bootloader.md)

## Chapter 7 - Creating The Root Filesystem

We will now create our root filesystem which we will use to install the
userspace software to the Nintendo 64.

### Chapter 7.1 - Preparing The Root Filesystem

TODO: install all the components of our root filesystem once we get a rootfs
on the cartridge working.  For now, we will assume that the rootfs directory
structure is already created in `${TUX64_BUILD_ROOT}/rootfs`.

### Chapter 7.2 - Creating The Root Filesystem Image

To use our root filesystem with Linux on the Nintendo 64, we need to embed it
as an image file into the cartridge.  Thus, we need to create a disk image.

First, we will create an empty file which we will create the root filesystem
image in.  This can be done with the following commands:

```
dd \
    if=/dev/zero \
    of=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/rootfs.img \
    bs=1M \
    count=32
```

This will create a 32MiB image file.  This should be plently enough for most
users.  Note that `bs=1M` means "block size of 1 mebibyte", and `count=32`
means "copy 32 blocks of size 1 mebibyte".  This should help if you would like
to create root filesystem images of different sizes.  Note, however, that the
root filesystem image must be aligned to a 4KiB boundary.

We will now create our filesystem within the image and install the root
filesystem.  We can accomplish this using the following command:

```
mke2fs \
    ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/rootfs.img \
    -d ${TUX64_BUILD_ROOT}/rootfs \
    -t ext4 \
    -b 4096
```

This will make an ext4 filesystem with a block size of 4KiB, and copy all files
from `${TUX64_BUILD_ROOT}/rootfs` into the root filesystem image.  While
theoretically we could use a block size of 1KiB, this would currently fail to
boot as the kernel expects a minimum block size of 4KiB.

Note that the above command does not create completely reproducible root
filesystem images.  This is because, by default, some randomness is used to
configure certain properties, such as the GUID.  For more information, it is
recommended to read the manpages for `mke2fs`.

We now have our root filesystem image created, and ready to be installed.
However, we still need a way to boot our kernel image with our root filesystem.

We will now proceed to [building the bootloader](08-building-the-bootloader.md).

