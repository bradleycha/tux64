# Tux64 Development Guide
<img src="../logo.png" width="156" height="156"/>

## Recalculating The `tux64-boot` Stage-0 CIC Data

If you are an end-user, you should not have to do this.  However, if you modify
a single byte of the stage-0 binary, it will cause the checksum calculated by
the PIF to be incorrect, thus the console won't boot.  You must recalculate the
CIC data in order to modify the stage-0 bootloader.

### Obtaining sources

The following tools are needed to re-calculate the stage-0 CIC data:

| Package | Version | Notes |
|---------|---------|-------|
| [ipl3hasher-new](https://github.com/Polprzewodnikowy/ipl3hasher-new) | 1.3.0 | This requires a Rust toolchain to be installed. |

### Building ipl3hasher-new

```
mkdir ${TUX64_BUILD_ROOT}/builds/ipl3hasher-new
cd ${TUX64_BUILD_ROOT}/builds/ipl3hasher-new

cargo build --release \
   -j${TUX64_MAKEOPTS} \
   --manifest-path=$(ls ${TUX64_BUILD_ROOT}/sources/ipl3hasher-new-*/Cargo.toml) \
   --target-dir=$(pwd)

cp release/ipl3hasher-new ${TUX64_BUILD_ROOT}/tools/bin/
```

You can now run ipl3hasher-new with the following command:

```
${TUX64_BUILD_ROOT}/tools/bin/ipl3hasher-new
```

### Calculating CIC data

We will now use ipl3hasher-new to brute-force the CIC data for force the stage-0 CIC checksum to match.
For now, we target the `CIC-6102` only, but future versions may target multiple CIC revisions.

```
${TUX64_BUILD_ROOT}/tools/bin/ipl3hasher-new \
   ${TUX64_BUILD_ROOT}/builds/tux64-rom-image/*.n64 \
   --cic 6102
```

When complete, you'll see output which looks like the following:

```
Found collision: Y=???????? X=????????
```

The `Y` value is the first 32-bit word for `.cicdata`, and `X` is the second.

