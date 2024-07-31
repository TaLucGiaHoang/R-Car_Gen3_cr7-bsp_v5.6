RENESAS R-Car CR7 Solution with RENESAS Android BSP Sample Software
===================================================================



1. Common overview
------------------
Renesas CR7 SDK supports several Guest OS (for today these are: Integrity, Linux
and Android). This subdirectory (`cr-firmware/android`) contains all the stuff
necessary for building Android with CR7 support.



2. Android SDK content
----------------------
The following part of the project is addressed to Android support.

```
cr-firmware
├── android
│   ├── build.sh        - main build script
│   ├── prebuilt        - directory with all output artifacts
│   ├── readme.txt      - this file
│   ├── setup-env.sh    - script for setup environment (should be called via
│   │                     `source` command every time)
│   ├── setup-repos.sh  - script for applying patches (should be called once)
│   ├── setup-tools.sh  - script for setup tools (should be called once)
│   └── work            - temporary directory used by the `build.sh` script
...
└── src                 - directories with the changes for CR7 support
    ...
    ├── android-10      - patches for Android 10
    ├── android-11      - patches for Android 11
    └── android-13      - patches for Android 13
...
<somewhere>/android
├── 10
│   └── mydroid         - Renesas Android BSP v10_2.0 (not a part of this SDK)
├── 11
│   └── mydroid         - Renesas Android BSP v11_1.1.3 (not a part of this SDK)
└── 13
    └── mydroid         - Renesas Android BSP v13_1.1 (not a part of this SDK)
```
Note, that SDK relies on reference Renesas Android BSP that is not a part of the
SDK, so it must be obtained separately.



3. Summary
----------
Below is a short summary of building and flashing procedures.
 - building:
```
$ ./setup-tools.sh
$ . setup-env.sh
$ ./build.sh cr7_ipl m3n
$ ./build.sh vlib m3n hello_rgl_wrapper
$ ./build.sh android-{10|11|13} m3n
$ ./build.sh release m3n
```
 - addresses for flashing the srec files:
```
bootparam_sa0.srec    e6320000  000000
cr7_loader.srec       e6304000  040000
cert_header_sa3.srec  e6320000  0C0000
bl2.srec              e6304000  140000
cert_header_sa6.srec  e6320000  180000
bl31.srec             44000000  1C0000
tee.srec              44100000  200000
u-boot-elf.srec       50000000  640000
cr7_rtos_app.srec     70000000  780000
```
 - flashing Android:
```
$ ./fastboot oem format
$ ./fastboot reboot bootloader
$ ./fastboot.sh --noresetenv
```

For details see below sections.



3. Preparation
--------------
Before building Android the SDK must be prepared.
Preparation includes three steps:
 - downloading and installing toolchains that are used for building CR7 loader
   and application;
 - set environment variables that are necessary for building;
 - apply changes to reference Android sources that are necessary for CR7
   support.

These steps are implemented by SDK build scripts: `setup-tools.sh`,
`setup-env.sh` correspondendly. The scripts must be called
one by one in the following way:

 - first the `setup-tools.sh` must be called:
```
$ ./setup-tools.sh
```
This script will download and install tools in the predefined directory. If the
default location for tools is not suitable it should be changed manually before
calling the script:
```
TOOLCHAIN_INSTALL_PATH=~/toolchain
```
This script should be called only once when the environment is prepared for
working with the SDK.

 - next the `setup-env.sh` must be executed:
```
$ source setup-env.sh
```
This script will export paths to the downloaded on the previous step tools and
path to the Android source tree. The script uses a predefined path for Android
sources. If the default location of Android sources is not suitable it should be
manually changed before calling the script:
```
export MYDROID_10=~/android/10/mydroid
export MYDROID_11=~/android/11/mydroid
export MYDROID_13=~/android/13/mydroid
```
This script must be called every time (for each new shell session) before
building Android.

 - eventually the `setup-repos.sh` will be called by the `build.sh` script:
This script modifies the reference Android sources by applying changes that are
necessary for CR7 support. The modifications include the RVGC (the Linux para
guest DRM driver) to replace the standard DU/VSPD DRM driver, VIVID (the Linux
para guest VIN driver). Also, the modifications change RPMSG, VIRTIO and MFIS
Kernel modules. All changes are provided as git patches (and stored in the
`src/android-XX` subdirectory). For Android projects that are provided a plain
copy (without a git repository), the script automatically initializes a new
repository (to make it possible to apply patches). Patches are applied via `git
am` command. If a patch can not be applied the script tries to apply changes
using the raw `patch` command. This measure can be useful when a patch contains
correct changes and can be applied by itself, but the git repository does not
have a correct (common) index for the patch. Before applying patches the script
stores all unsaved working changes by creating an automatic commit. Then the
script creates a new branch for CR7 changes and checks out on this branch.
This script should be called only once when the default Renesas Android BSP is
being modified to support CR7 (to rebuild it mustn't be called again).



4. Building
-----------
The building procedure includes three steps:
 - building the CR7 IPL loader;
 - building the CR7 application;
 - building Android images.

These steps are implemented by the SDK `build.sh` script that must be called
several times with different options in the following way:
 - first CR7 loader should be built:
```
$ ./build.sh cr7_ipl m3n
```

 - then CR7 application has to be built:
```
$ ./build.sh vlib m3n hello_rgl_wrapper
```
This step is obligatory for building Android. This is because the Android boot
image (Linux Kernel) must contain a CR7 firmware file. While the firmware file
is a CR7 application image in the ELF format.
Note, by default applications are built using FreeRTOS, to build a use case with
Trampoline OS add a `trampoline` parameter, like:
```
$ ./build.sh vlib m3n logo trampoline
```

 - eventually Android can be built:
```
$ ./build.sh android-{version} m3n
```
where `version` is one from the supported list: 10, 11 or 13.
The `build.sh` script sets the necessary options for building Android with CR7
support and calls the make command.
Note that Renesas Android BSP environment shall be prepared based on the Release
Notes in advance (RENESAS_RCH3M3M3N_Android_11_ReleaseNote_2021_03E.pdf for
Android 11 and RENESAS_RCH3M3N_Android_13_ReleaseNote_2023_6E.pdf for Android 13).



5. Packaging
------------
For convenience the `build.sh` script allows preparing a package with all
necessary artifacts that include IPLs and CR7 srec files, Android images and
flashing script. The following command will create the package and put it in the
`android/prebuilt` subdirectory:
```
$ ./build.sh release m3n
```



6. Flashing
-----------
The flashing procedure includes two steps:
 - flashing IPL and CR7 srec files;
 - flashing Android images.

IPLs and CR7 application srec files can be flashed in any convenient way.
The following addresses must be used for flashing:
```
bootparam_sa0.srec    e6320000  000000
cr7_loader.srec       e6304000  040000
cert_header_sa3.srec  e6320000  0C0000
bl2.srec              e6304000  140000
cert_header_sa6.srec  e6320000  180000
bl31.srec             44000000  1C0000
tee.srec              44100000  200000
u-boot-elf.srec       50000000  640000
cr7_rtos_app.srec     70000000  780000

```
The following switch (for Salvator-XS board) mode must be used:
 - for flashing: SW10 (++++----);
 - for booting:  SW10 (--++++-+).
Refer to Yocto Startup Guide for more details
(RENESAS_RCH3M3M3NE3_YoctoStartupGuide_UME_v3.21.0.pdf for Android 11 and
RENESAS_RCH3M3M3NE3D3_YoctoStartupGuide_UME_v5.9.0.pdf for Android 13).

Flashing Android images is performed according to the Renesas Android BSP
Release Notes: RENESAS_RCH3M3M3N_Android_11_ReleaseNote_2021_03E.pdf or
RENESAS_RCH3M3N_Android_13_ReleaseNote_2023_6E.pdf correspondingly. In short,
the following commands should be done:
```
$ ./fastboot oem format
$ ./fastboot reboot bootloader
$ ./fastboot.sh --noresetenv
```
If necessary eMMC partitions can be erased according to the Release Notes.



7. Running
----------
To run sample application set switch to boot from CR7 processor (see previous
section) and connect a display to HDMI0 port. After that power on a board. CR7
application will start.
To run Android the following U-Boot parameters should be set:
```
bootcmd=boota 1 boot 0x4C000000
bootargs=androidboot.display.res.HDMI1=1024x768-32@60
```
Note, resolution has to match value set in the DTS (rvgc item description).



8. Special notes for Android 10
-------------------------------
Android 10 uses old implementation of the Preparation step that does not match
description in the section "3. Preparation".



9. Special notes for Android 11
-------------------------------
Known Issues and Limitations:
1. In the Android EVS HAL, the stride value for producer (rcar-vivid) and
consumer (gralloc) may be different. As a workaround for such issue a simple
image correction with CPU usage is implemented.
2. Simultanious usage of Android EVS HAL and CR7 rearview_camera application
since both application requires the same camera stream. The Android EVS
application can be used with other use cases, for example with the logo one.
3. While HDMI display has five layers, LVDS and DPAD has three (0, 1, 2) and two
(3, 4) layers correspondently (because they share the same VSPD unit). This is a
hardcoded configuration for the current CR7 SDK implementation. And this
information should be taking into an account while modifying the rvgc node
settings in the Linux Kernel DTS.



10. Special notes for Android 13
--------------------------------
1. The current version of the Android 13 supports only RVGC use case.
2. The build script doesn't call automatically the `setup-repos.sh` script
for Android 13. So it must be called manually before building.
3. The current version expects that the CR7 ELF file doesn't contain the
CARVEOUT section. If the CARVEOUT section is present it may lead to CMA
allocation failure and as a result, Android can't successfully boot.
4. The Android 13 has the same limitation as Android 11 item 3.
