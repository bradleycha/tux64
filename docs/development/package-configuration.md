# Tux64 Development Guide
<img src="../logo.png" width="156" height="156"/>

## Package Configuration

Some packages can be configured to be more debug-friendly, or flat-out require
certain options to make debugging possible.

### Changes to ```buildconf.sh```

It's recommended to make the following changes to `buildconf.sh` to aid in
debugging:

| Name | Value |
|------|-------|
| TUX64_CFLAGS_HOST | -pipe -march=native -O0 -g -fsanitize=undefined |
| TUX64_LDFLAGS_HOST | -g |
| TUX64_CFLAGS_N64_COMMON |  -pipe -march=vr4300 -mfix4300 -O0 -g |
| TUX64_LDFLAGS_N64_COMMON | -g |
| TUX64_CFLAGS_N64_BOOTLOADER | ${TUX64_CFLAGS_N64_COMMON} -mabi=o64 |

You may either overwrite `buildconf.sh`, or save the modified version to a new
file, such as `buildconf-dev.sh` so you may use the development
`buildconf-dev.sh` only for Tux64 code.  This may be desirable for compiling
external software and libraries with optimized compiler flags, while still
retaining enhanced debug support for internal code.

### Enabling Dependency Tracking

If you look closely, the `configure` commands from the installation guide
include `--disable-dependency-tracking`.  This is done to skip all dependency
tracking for sources files in order to speed up compilation.  Dependency
tracking is used to detect if a file's dependencies are modified, and if so,
recompile that file.  For example, if a C header file is modified, all C
source files which include that header will be recompiled.  For end-users, this
functionality serves no purpose but to slow down compilation.  However, for
developers, this is a must if you don't want to constantly run `make clean`
before each recompile.

To enable dependency tracking, simply replace all instances of
`---disable-dependency-tracking` with `--enable-dependency-tracking`, or omit
both as the default is to enable dependency tracking.

### Building Kernel Debug Symbols

If you are looking to do kernel debugging, you will need debug symbols.  These
are normally enabled via our modified `buildconf.sh`, however Linux needs them
to be enabled manually.

```
Kernel hacking  --->
  Compile-time checks and compiler options  --->
    Debug information  --->
      (X) Rely on the toolchain's implicit default DWARF version
```

Now recompile and install the kernel, as well as rebuild the ROM image as
detailed in the installation guide.

### Enabling `tux64-lib` Log Origin

`tux64-lib`'s logging functions include support to include the source file and
line number each log message originates from.  This can be useful for
development to quickly track down specific log messages without needing a
debugger attached.

To do this, reconfigure `tux64-lib` with `---enable-log-origin`.

### Enabling `tux64-boot` Debug Assertions

`tux64-boot` can be optionally configured with `--enable-debug`.  This enables
some safety checks which can catch programming mistakes at the cost of some
runtime overhead.

This may be useful for bootloader development, where simple programming mistakes
will cause the bootloader to safely halt instead of violently exploding, aiding
in debugging.

### Enabling Boot Status Codes

`tux64-boot` can be optionally configured with `--enable-status`.  This will
write status code information to a fixed memory address at the end of RSP DMEM,
specifically 8 bytes to `0xa4000ff8`.  The bytes are ASCII characters which
spell out `STAGEn:p`, where "n" is the bootloader stage and "p" is the location
last reached within the relevant bootloader stage.

These may be useful for extremely low-level debugging, such as debugging very
early boot failures on real hardware before the remote debugger is available by
probing the SysAD bus with a logic analyzer.  Otherwise, these serve no purpose
and can be safely left disabled.

### Enabling Dumping of Register File and Status Codes to Cartridge SRAM.

`tux64-boot` can be optionally configured with `--enable-dump-to-sram`.  This
will add the ability to dump the entire general-purpose register file and
current status code to cartridge SRAM with the macro
`TUX64_BOOT_STAGE0_DUMP_TO_SRAM`.

This can be used to aid in stage-0 debugging without the need of a logic
analyzer as detailed above.  However, it will require a flashcart with the
ability to dump save files, such as a SummerCart64.

Note that this configuration option will break the brute-forced IPL2 checksum
value.  This may be a problem with some flashcarts and require computing a new
checksum value, but your mileage may vary.

