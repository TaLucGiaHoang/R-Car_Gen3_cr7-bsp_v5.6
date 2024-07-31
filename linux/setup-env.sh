#!/bin/bash
# Renesas R-Car CR7 Solution
#
# Set up environment before running build.sh.
# Use '$ source setup-env.sh'

# TODO: Change toolchain as needed.
export PATH=~/toolchain/gcc-arm-8.2-2018.11-x86_64-arm-eabi/bin:$PATH
export PATH=~/toolchain/gcc-arm-none-eabi-7-2018-q2-update/bin/:$PATH
export PATH=~/toolchain/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin:$PATH
export PATH=~/toolchain/cmake/bin:$PATH
export SOURCE_DATE_EPOCH=$(date +%s)

# TODO: Set the location of Renesas Yocto 5.9 BSP if used otherwise uncomment it
export YOCTO_BSP_5_9=~/R-Car_Gen3_Evaluation_Software_Package_for_Linux_for_Yocto_v5.9.0
# TODO: Set the location of the Yocto building directory if used otherwise uncomment it
export YOCTO_DIR=~/Yocto-5.9