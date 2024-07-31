# CR7 Safe Rendering Software

This file is a "quick start" description. Please see docs/R-Car Cortex-R7 Reference Software Startup Guide.pdf for more details.

## Pre-requisites
### Large File Storage (RENESAS internal only)
This project uses `git lfs` for large files.  

### Cloning the software (RENESAS internal only)
This project uses `git lfs` for large files. To communication to the server containing the files uses http, not SSH. You will not be able to successfully clone the project without special certificates installed.  

Clone using the following command to ignore SSL verification:  
`git -c http.sslverify=false clone git@rcar-env.dgn.renesas.com:rcar-reference-sw/cr-firmware.git`

To install git-lfs: `sudo apt install git-lfs`

### Initialize submodules (RENESAS internal only)
This project uses several submodules for external software components. You need to initialize the submodules first:
```
git submodule update --init --recursive
```

### Troubleshooting project setup (RENESAS internal only)
If git lfs is not installed prior to cloning, or for a pre-existing repository, follow the commands below to install git-lfs and pull the files:
```
sudo apt update
sudo apt install git-lfs
git-lfs fetch --all
git-lfs pull
```

### Compiler
For the CR7 (arm-gnu) the following toolchains have been used:

1) “7-2018-q2-update” downloaded from https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
2) Ubuntu 16.04 stock gcc-arm-none-eabi
2) Ubuntu 18.04 stock gcc-arm-none-eabi

### CMake
This project is built using the CMake system. CMake versions 3.16 and 3.21 have been tested. Download CMake 3.16 with the follow command:
```
mkdir ~/toolchains
cd ~/toolchains
curl -OL https://cmake.org/files/v3.16/cmake-3.16.9-Linux-x86_64.sh && chmod +x cmake-3.16.9-Linux-x86_64.sh && ./cmake-3.16.9-Linux-x86_64.sh --skip-license
```
*Note: Don't forget to add `~/toolchains/bin` to your `PATH`*

## ZIP package setup
If you are using the .zip package, you can prepare the CR7 sources by executing `./patches/setup.sh` 

This will download FreeRTOS and Trampoline, and apply patches from Renesas.

## Building the software components
### Building CR7 bootloader
Please refer to instructions in the `loader/doc` directory.

### Building Yocto Linux
Please refer to instructions in the `yocto/README.md`.

### Build hello_rgl example
```
$ mkdir build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE="compiler/arm-gnu.cmake" -DRCAR_TARGET_OS=freertos -DRCAR_SET_FPU=hard -DRCAR_TARGET_GUEST=LINUX_GUEST ../src/vlib
$ cmake --build . --parallel $(nproc) --target hello_rgl_wrapper
# or using make directly (not recommended)
$ make -j$(nproc) hello_rgl_wrapper
```

Output files (`.out` ELF files and `.srec` files) are located in `<build dir>/bin/`

* `RCAR_TARGET_OS` must be `freertos` or `trampoline`
* `RCAR_TARGET_GUEST` must be `LINUX_GUEST` or `INTEGRITY_GUEST`
* `RCAR_SET_FPU` must be `hard` or `soft`
* `make` without a target will build all applications

### Build Android + CR7
*Note: set the correct `path` to the Android project in the file `setup-env.sh` before building!*
Use the following commands for building:
```
$ ./setup-tools.sh
$ . setup-env.sh
$ ./setup-repos.sh
$ ./build.sh cr7_ipl m3n
$ ./build.sh vlib m3n logo
$ ./build.sh android-11 m3n
$ ./build.sh release m3n
```

### Build Yocto 5.9 + CR7
*Note: set the correct `path` to the Yocto 5.9 BSP and Yocto building directory in the file `setup-env.sh` before building!*

Use the following commands for building:
```
$ ./setup-tools.sh
$ . setup-env.sh
$ ./setup-repos.sh
$ ./build.sh cr7_ipl m3n
$ ./build.sh vlib m3n logo
$ ./build.sh linux m3n
$ ./build.sh release m3n
```

Output files ( Android `.img` and `.srec` files) are located in `../android/prebuilt/LSI-Android-version/`)

### Complete Quick Start
Use the following commands to quickly setup and build the environment:
```
# setup.sh is only used for .zip package release
$ ./patches/setup.sh
$ ./linux/setup-tools.sh
$ . setup-env.sh
```

That's it! Now, you can build CR7 applications. You may use either the build.sh script, or manually with CMake commands. 

This package also supports VSCode, which you can use to select specific build "kits" as well as building specific library / executables using the UI (need CMake extension installed) 

Refer to the above sections for details.
