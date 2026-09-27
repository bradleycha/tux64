# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 6 - Building The Kernel](06-building-the-kernel.md)
* Next Page: [Chapter 8 - Creating The Root Filesystem](08-creating-the-root-filesystem.md)

## Chapter 7 - Creating The Initramfs

TODO: figure out what exactly we need in our initramfs, if we even need one at
all.  for now, just do the following:

```
touch ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER}/boot/initramfs.cpio
```

We will now proceed to [creating the root filesystem](08-creating-the-root-filesystem.md).

