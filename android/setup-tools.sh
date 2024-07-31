#!/bin/bash
# Renesas R-Car CR7 Solution
#
# Download and unpack toolchains.
set -e

# Change this as needed
TOOLCHAIN_INSTALL_PATH=~/toolchain

# Set up toolchain location
TOOLCHAIN_INSTALL_PATH=$(readlink -f $TOOLCHAIN_INSTALL_PATH)
mkdir -p ${TOOLCHAIN_INSTALL_PATH}
cd ${TOOLCHAIN_INSTALL_PATH}

# Install bare-metal toolchain for CR7 code
wget -O gcc-arm-8.2-2018.11-x86_64-arm-eabi.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu-a/8.2-2018.11/gcc-arm-8.2-2018.11-x86_64-arm-eabi.tar.xz?revision=e5d50023-0e75-44ab-9e23-9cc71233665a&la=en"
tar xf gcc-arm-8.2-2018.11-x86_64-arm-eabi.tar.xz

# Install toolchain for CR7 rcar-vlib
wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2
tar xf gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2

# Install toolchain for CA5x code
wget https://releases.linaro.org/components/toolchain/binaries/7.3-2018.05/aarch64-linux-gnu/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu.tar.xz
tar xf gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu.tar.xz

# Install toolchain for cr-firmware
curl -OL https://cmake.org/files/v3.16/cmake-3.16.9-Linux-x86_64.sh \
	&& chmod +x cmake-3.16.9-Linux-x86_64.sh \
	&& mkdir $(pwd)/cmake \
	&& ./cmake-3.16.9-Linux-x86_64.sh --prefix=$(pwd)/cmake --skip-license
