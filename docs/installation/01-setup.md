# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 0 - Introduction](00-introduction.md)
* Next Page: [Chapter 2 - Obtaining Sources](02-obtaining-sources.md)

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

We will now proceed to [obtaining package sources](02-obtaining-sources.md).

