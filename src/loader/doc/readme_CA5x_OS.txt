Cortex-R7 Loader (rev.2.0.9)
How to update load address for Android and Linux
                                                                      2022.11.08

1. Overview

This document describes how to update the load address of CA5x OS in Cortex-R7 Loader's source code for Android and Linux.


2. Build command

Add below flag to make command to build it for Linux or Android.

For Linux - It is default that does not need any flag to be set explicitly 
Alternatively, RCAR_CA5x_OS=1 can also set load address for Linux

For Android - 
RCAR_CA5x_OS=2

e.g. 
For Linux -
CROSS_COMPILE=~/Tools/gcc-linaro-7.3.1-2018.05-x86_64_arm-eabi/bin/arm-eabi- make LSI=H3 RCAR_DRAM_SPLIT=1 RCAR_KICK_MAIN_CPU=1

CROSS_COMPILE=~/Tools/gcc-linaro-7.3.1-2018.05-x86_64_arm-eabi/bin/arm-eabi- make LSI=H3 RCAR_DRAM_SPLIT=1 RCAR_KICK_MAIN_CPU=1 RCAR_CA5x_OS=1

For Android -
CROSS_COMPILE=~/Tools/gcc-linaro-7.3.1-2018.05-x86_64_arm-eabi/bin/arm-eabi- make LSI=H3 RCAR_DRAM_SPLIT=1 RCAR_KICK_MAIN_CPU=1 RCAR_CA5x_OS=2

