# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 7 - Creating The Root Filesystem](07-creating-the-root-filesystem.md)
* Next Page: [Chapter 9 - Creating The ROM Image](09-creating-the-rom-image.md)

## Chapter 8 - Building The Bootloader

We now have a kernel image and a bunch of software, but how do we boot this?
With a bootloader, of course!  Specifically, `tux64-boot`.  This will produce a
series of binary "stage" files.  These are the executable code which will run
right after the console powers on and starts executing code from the cartridge.

For this, we have to choose which console region we want to support, or include
support for multiple regions if desired.  We can either support PAL (European
N64s), NTSC (American and Japanese N64s), or M-PAL (Brazilian N64s?), or
alternatively only support the iQue player, which is treated as its own
"region".  Including support for multiple regions is useful if you want a single
unified bootloader that supports all regions and consoles, at the cost of some
slight bloat.

For most people, you will only need NTSC support.  To do this, configure with
`--enable-region-ntsc`.  For other regions, refer to the help menu from the
`configure` script with `--help`.

**Important note**:  If you try to use the bootloader with an unsupported
console region, you will get a black screen.

In addition, you can configure the color palette for the on-screen console.  By
default, it uses white text on a black background.  To customize this, set the
`${CONFIG_COLOR_FOREGROUND}` and `${CONFIG_COLOR_BACKGROUND}` to one of the
pre-defined colors.  This can be any of the following:

- `white`
- `black`
- `gray` or `grey`
- `magenta`
- `red`
- `orange`
- `yellow`
- `green`
- `cyan`
- `blue`
- `indigo`
- `violet`
- `purple`
- `ruby`
- `emerald`
- `sapphire`

There are a few other configuration options, which you can find more information
about in the `configure` script's help menu.  The provided configuration here is
a "full-featured" build, which should support general use-cases.  For those
looking to squeeze every byte out of their console, you may want to consider
disabling unused features.

```
mkdir ${TUX64_BUILD_ROOT}/builds/tux64-boot
cd ${TUX64_BUILD_ROOT}/builds/tux64-boot

(
   . ${TUX64_BUILD_ROOT}/scripts/usetoolchain.sh \
      ${TUX64_BUILD_ROOT}/tools/bin/${TUX64_TARGET_N64_BOOTLOADER}
   ../../sources/tux64-*/boot/configure \
      --disable-dependency-tracking \
      --host=${TUX64_TARGET_N64_BOOTLOADER} \
      --prefix=${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_BOOTLOADER} \
      CFLAGS="${TUX64_CFLAGS_N64_BOOTLOADER}" \
      ASFLAGS="${TUX64_ASFLAGS_N64_BOOTLOADER}" \
      LDFLAGS="${TUX64_LDFLAGS_N64_BOOTLOADER}" \
      TUX64_FONTCOMPILER=${TUX64_BUILD_ROOT}/tools/bin/tux64-fontcompiler \
      TUX64_TEXTCOMPILER=${TUX64_BUILD_ROOT}/tools/bin/tux64-textcompiler \
      TUX64_IMAGECOMPILER=${TUX64_BUILD_ROOT}/tools/bin/tux64-imagecompiler \
      CONFIG_COLOR_FOREGROUND=white \
      CONFIG_COLOR_BACKGROUND=black \
      --enable-power-management \
      --enable-rsp-dma-double-buffering \
      --enable-status \
      --enable-checksum \
      --enable-delay \
      --enable-splash \
      --enable-logo \
      --enable-memory-display \
      --enable-region-pal \
      --enable-region-ntsc \
      --enable-region-mpal \
      --enable-ique
)

make -j${TUX64_MAKEOPTS}
make -j${TUX64_MAKEOPTS} install
```

We will now proceed to [creating the ROM image](09-creating-the-rom-image.md).

