# Tux64 Installation Guide

* Previous Page: [Chapter 5 - Building The Kernel](05-building-the-kernel.md)
* Next Page: [Chapter 7 - Creating The Root Filesystem](07-creating-the-root-filesystem.md)

## Chapter 6 - Creating The Initramfs

TODO: figure out what exactly we need in our initramfs, if we even need one at
all.  for now, just do the following:

```
touch ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/initramfs.cpio
```

We will now proceed to [creating the root filesystem](07-creating-the-root-filesystem.md).

