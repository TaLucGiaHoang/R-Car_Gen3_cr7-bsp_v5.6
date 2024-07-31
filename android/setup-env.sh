#!/bin/bash
# Renesas R-Car CR7 Solution
#
# Set up environment before running build.sh.
# Use '$ source setup-env.sh'

# Change toolchain as needed.
export PATH=~/toolchain/gcc-arm-8.2-2018.11-x86_64-arm-eabi/bin:$PATH
export PATH=~/toolchain/gcc-arm-none-eabi-7-2018-q2-update/bin/:$PATH
export PATH=~/toolchain/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin:$PATH
export PATH=~/toolchain/cmake/bin:$PATH

# Set the location of Renesas Android BSP and uncomment it
#export MYDROID_10=~/android/10/mydroid
#export MYDROID_11=~/android/11/mydroid
#export MYDROID_13=~/android/13/mydroid

export ANDROID11_PKG_NAME="Gen3_Android_v11_1.1.3.zip"
export ANDROID11_PKG_DIR=~/Android11/test
