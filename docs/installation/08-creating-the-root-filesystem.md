# Tux64 Installation Guide
<img src="../logo.png" width="156" height="156"/>

* Previous Page: [Chapter 7 - Creating The Initramfs](07-creating-the-initramfs.md)
* Next Page: [Chapter 9 - Building The Bootloader](09-building-the-bootloader.md)

## Chapter 8 - Creating The Root Filesystem

We will now create our root filesystem which we will use to install the
userspace software to the Nintendo 64.

### Chapter 8.1 - Installing The Userspace Software

We will now install all the software we built in
[Chapter 5](05-building-userspace-software.md).  We manually install because
there tends to be many unnecessary build files which are installed by default,
such as static libraries.  These do nothing but bloat our already limited root
filesystem, thus we only install what needs to be installed.

First, we need to create the base directory structure.

```
cd ${TUX64_BUILD_ROOT}/rootfs
mkdir -p usr/bin usr/lib usr/libexec usr/share etc home root
ln -s usr/bin bin
ln -s usr/bin sbin
ln -s usr/lib lib
ln -s usr/libexec libexec
```

We will now start installing all of our built userspace software.

#### Chapter 8.1.1 - Installing `gcc`

While `gcc` is a compiler which runs on the host, it also contains support
libraries which must be linked to userspace applications at runtime.

```
cd ${TUX64_BUILD_ROOT}/rootfs/lib
cp ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/lib/libgcc*.so* ./
```

#### Chapter 8.1.2 - Installing `musl`

```
cd ${TUX64_BUILD_ROOT}/rootfs/lib
cp ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/lib/libc.so ./
ln -s libc.so ld-musl-mips.so.1
```

#### Chapter 8.1.3 - Installing `tux64-lib`

```
cd ${TUX64_BUILD_ROOT}/rootfs/lib
cp ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/lib/libtux64-0.1.0+setup.so ./
ln -s libtux64-0.1.0+setup.so libtux64.so
```

#### Chapter 8.1.3 - Installing `tux64-init`

```
cd ${TUX64_BUILD_ROOT}/rootfs/sbin
cp ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/bin/tux64-init ./
ln -s tux64-init init
```

#### Chapter 8.1.4 - Installing `zlib`

```
cd ${TUX64_BUILD_ROOT}/rootfs/lib
cp ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/lib/libz.so.1.3.2 ./
ln -s libz.so.1.3.2 libz.so.1
ln -s libz.so.1.3.2 libz.so
```

#### Chapter 8.1.5 - Installing `openssl`

```
cd ${TUX64_BUILD_ROOT}/rootfs/lib
for lib in libcrypto.so.4 libssl.so.4; do
    cp ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/lib/$lib ./
done
ln -s libcrypto.so.4 libcrypto.so
ln -s libssl.so.4 libssl.so
```

#### Chapter 8.1.6 - Installing `gmp`

TODO: document this once we get `gmp` to build.

#### Chapter 8.1.7 - Installing `coreutils`

```
cd ${TUX64_BUILD_ROOT}/rootfs
for bin in \[ b2sum base32 base64 basename basenc cat chgrp chmod chown chroot\
    cksum comm cp csplit cut date dd df dir dircolors dirname du echo env\
    expand expr factor false fmt fold groups head hostid id install join link\
    ln logname ls md5sum mkdir mkfifo mknod mktemp mv nice nl nm nohup nproc\
    numfmt od paste pathchk pinky pr printenv printf ptx pwd readlink realpath\
    rm rmdir seq sha1sum sha224sum sha256sum sha384sum sha512sum shred shuf\
    sleep sort split stat stdbuf stty sum sync tac tail tee test timeout touch\
    tr true truncate tsort tty uname unexpand uniq unlink users vdir wc whoami\
    yes; do
    cp ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/bin/$bin ./bin/
done
cp -r ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/libexec/coreutils ./libexec/
```

#### Chapter 8.1.8 - Installing `bash`

```
cd ${TUX64_BUILD_ROOT}/rootfs/bin
cp ${TUX64_BUILD_ROOT}/tools/${TUX64_TARGET_N64_LINUX}/bin/bash ./
ln -s bash sh
```

### Chapter 8.2 - Configuring The Root Filesystem

We now need to create configuration files which will be used by our userspace
software.

TODO: document configuration, such as `.bashrc`.

### Chapter 8.3 - Creating The Root Filesystem Image

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
${TUX64_BUILD_ROOT}/tools/bin/mke2fs \
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

We will now proceed to [building the bootloader](09-building-the-bootloader.md).

