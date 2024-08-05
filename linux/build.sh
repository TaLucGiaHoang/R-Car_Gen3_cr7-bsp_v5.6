#!/bin/bash
# Renesas R-Car CR7 Solution - Build everything
#
# Note this script relies on having two ARM cross-compilers installed.
# Unfortunately, various software needs CROSS_COMPILE and ARCH env vars, but we
# have to set them differently. The name triplets sometimes change as well.
# See below...
#
# Notes:
# o FreeRTOS fails to build with gcc-linaro-7.3.1-2018.05-x86_64_arm-eabi

# default
LSI=M3
BOARD=salvatorxs
RTOS=freertos
FPU=hard
R_USE_VIN=OFF
USE_HDMI0=OFF
VLIBAPP=fastboot_mono
CR7_IPL_DBG=0

# Arg handling
POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
	ipl|cr7_ipl|cr7_loader)
	CR7_IPL=true
	shift
	;;
	debugger)
	CR7_IPL_DBG=1
	shift
	;;
	vlib)
	VLIB=true
	shift
	;;
	trampoline|freertos)
	RTOS=$1
	shift
	;;
	l|linux)
	LINUX=true
	shift
	;;
	nopatch)
	NOPATCH=true
	shift
	;;
	all)
	CR7_IPL=true
	VLIB=true
	LINUX=true
	shift
	;;
	release)
	RELEASE=true
	shift
	;;
	h3|H3|m3|M3|m3n|M3N|e3|E3)
	LSI=${1^^}
	shift
	;;
	ebisu|salvatorxs|erguotou)
	BOARD=$1
	shift
	;;
	clean)
	CLEAN=true
	shift
	;;
	help|-h|h|?)
	echo "--- Usage ---"
	echo "$0 help"
	echo "$0 all|cr7_ipl|vlib|linux|release [h3|m3|m3n|e3] [board] [trampoline|freertos] [clean]"
	echo "$0 vlib [h3|m3|m3n|e3] [salvatorxs|ebisu|erguotou] [trampoline|freertos] [app_name] [clean]"
	echo "Example:"
	echo "1) $0 cr7_ipl m3n        -- Build bootloader (cr7_ipl) for M3N"
	echo "2) $0 vlib m3n hello_rgl -- Build vlib fastboot_mono for M3N"
	echo "3) $0 linux m3n          -- Build Yocto for M3N"
	echo "4) $0 clean              -- Clean all build"
	echo "5) $0 release m3n        -- Release bins and images for M3N to prebuilt folder"
	echo ""
	shift
	exit
	;;
	*)
	VLIBAPP=${1}
	shift
	;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

# Script directory
cd ..
ROOT_DIR=$(dirname $(readlink -f $0))
cd linux/

# Default working dir
if [[ -z "${WORK}" ]]; then
	WORK=./work
fi
WORK=$(readlink -f $WORK)

# Make output directory
mkdir -p ${WORK}/output/cr7/${LSI}/bin
mkdir -p ${WORK}/output/cr7/${LSI}/elf
mkdir -p ${WORK}/output/cr7/${LSI}/srec

# CR7 apps are binary compatible, so make all folder
mkdir -p ${WORK}/output/cr7/all/bin
mkdir -p ${WORK}/output/cr7/all/elf
mkdir -p ${WORK}/output/cr7/all/srec

cd ${WORK}

# Some of the software only appends to make implicit variables, so make sure they are not set
unset CFLAGS
unset CPPFLAGS
unset LDFLAGS

#################### Arm Cortex A5x code ##############################
# Linaro toolchain
export CROSS_COMPILE="aarch64-linux-gnu-"
# Poky toolchain
#export CROSS_COMPILE="aarch64-poky-linux-"
export ARCH=arm64

# Sample command:
# 1) ./build.sh vlib m3 fasboot_mono
# 2) ./build.sh vlib h3 cluster_2dwow trampoline erguotou
if [ "$VLIB" = true ] ; then
	echo; echo; echo "R-Car CR7 Solution: Building R-Car VLIB $VLIBAPP with $RTOS on $BOARD $LSI"
	mkdir -p ${WORK}/cr7-vlib-$RTOS
	cd ${WORK}/cr7-vlib-$RTOS
	if [ "$CLEAN" = true ] ; then
		make clean
	else
		if [ "$RTOS" = trampoline ] && [ "$VLIBAPP" != rivp_sample ] ; then
			FPU=soft
		fi
		if [ "$VLIBAPP" == "rearview_camera" ] ; then
			R_USE_VIN=ON
		fi
		if [ "$VLIBAPP" == "logo" ] ; then
			USE_HDMI0=ON
		fi
		cmake -DCMAKE_TOOLCHAIN_FILE="compiler/arm-gnu.cmake" -DRCAR_TARGET_OS=${RTOS} -DRCAR_TARGET_GUEST=LINUX_GUEST \
				-DRCAR_SET_FPU=${FPU} -DUSE_HDMI0=${USE_HDMI0} -DR_USE_VIN=${R_USE_VIN} ${WORK}/../../src/vlib
		make -j$(nproc) ${VLIBAPP}
		cd bin
		cp ${VLIBAPP}.out cr7_rtos_app.out

		${CROSS_COMPILE}objcopy --srec-forceS3 -O srec cr7_rtos_app.out cr7_rtos_app.srec
		mv cr7_rtos_app.srec ${WORK}/output/cr7/all/srec/
		mv cr7_rtos_app.out ${WORK}/output/cr7/all/elf/
	fi
	cd ..
fi

###################### Arm Cortex R7 code #############################
export CROSS_COMPILE="arm-eabi-"
#export CROSS_COMPILE="arm-none-eabi-"
export ARCH=arm

# Sample command:
# 1) ./build.sh cr7_ipl m3n
# 2) ./build.sh cr7_ipl m3 debugger
if [ "$CR7_IPL" = true ] ; then
	echo; echo; echo "RCar Gen3 CR7 Solution: Building CR7 IPL"
	echo; echo; echo "Please note that these are defualt settings and may need to be changed based on specific developement board configuration, e.g. 4G vs 8G RAM"
	echo; echo; echo "IPL configuration settings can be found in IPL specification in ../src/loader/doc folder."
	cd ${WORK}/../../src/loader/src/CortexR7_Loader
	if [ "$CLEAN" = true ] ; then
		make LSI=${LSI} clean
	else
		# Set 'LOG_LEVEL=40' to output debug info and 'LOG_LEVEL=0' to output none
		# Set 'RCAR_LOAD_RTOS_FROM_DEBUGGER=1' to wait for debugger attachment.
		make LSI=${LSI} clean
		if [ "$LSI" = "M3" ] ; then
			#1.3 (DRAM split - 1, DRAM LPDDR4 - 0)
			#make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=0 RCAR_DRAM_LPDDR4_MEMCONF=0 LOG_LEVEL=0 RCAR_CA5x_OS=1
			#3.0
			make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=2 RCAR_DRAM_LPDDR4_MEMCONF=1 LOG_LEVEL=20 RCAR_CA5x_OS=1
		elif [ "$LSI" = "M3N" ] ; then
			#1.1 (DRAM split - 3, LPDDR4 - 2 (for M3N 4GB))
			CROSS_COMPILE=arm-eabi- make LSI=${LSI} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=3 RCAR_DRAM_LPDDR4_MEMCONF=2 RCAR_CA5x_OS=1
			#1.1 (DRAM split - 3, LPDDR4 - 1 (for M3N 2GB))
			#make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=3 RCAR_DRAM_LPDDR4_MEMCONF=1 LOG_LEVEL=20 RCAR_CA5x_OS=1
		elif [ "$LSI" = "H3" ] ; then
			make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=1 RCAR_DRAM_LPDDR4_MEMCONF=1 LOG_LEVEL=20 RCAR_CA5x_OS=1
		elif [ "$LSI" = "E3" ] ; then
			# Ebisu-4D
			make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=2 RCAR_SA0_SIZE=0 RCAR_DRAM_DDR3L_MEMCONF=1 RCAR_DRAM_DDR3L_MEMDUAL=1 LOG_LEVEL=20 RCAR_CA5x_OS=1
		fi

		cp bootparam_sa0.bin cert_header_sa3.bin cr7_loader.bin ${WORK}/output/cr7/${LSI}/bin
		cp bootparam_sa0.srec cert_header_sa3.srec cr7_loader.srec ${WORK}/output/cr7/${LSI}/srec
		cp bootparam_sa0.elf cert_header_sa3.elf cr7_loader.elf ${WORK}/output/cr7/${LSI}/elf
	fi
	cd ..
fi

# Sample command:
# 1) ./build.sh linux m3n
if [ "$LINUX" = true ] ; then
	echo; echo; echo "Renesas R-Car CR7 Solution: Building Yocto"
	# This sets the SoC                                                              
 	# H3: r8a7795, M3: r8a7796, M3N: r8a77965 
	if [ "$LSI" = "H3" ] ; then
		echo; echo 'Changing SOC_FAMILY to "r8a7795"'
		cd ${YOCTO_DIR}/build/conf/
		sed -i '/SOC_FAMILY/d' local.conf
		sed -i 's/# H3: r8a7795, M3: r8a7796, M3N: r8a77965/# H3: r8a7795, M3: r8a7796, M3N: r8a77965\nSOC_FAMILY = "r8a7795"\n#SOC_FAMILY = "r8a7796"\n#SOC_FAMILY = "r8a77965"/g' local.conf
	elif [ "$LSI" = "M3N" ] ; then
		echo; echo 'SOC_FAMILY changed to "r8a77965"'
		cd ${YOCTO_DIR}/build/conf/
		sed -i '/SOC_FAMILY/d' local.conf
		sed -i 's/# H3: r8a7795, M3: r8a7796, M3N: r8a77965/# H3: r8a7795, M3: r8a7796, M3N: r8a77965\n#SOC_FAMILY = "r8a7795"\n#SOC_FAMILY = "r8a7796"\nSOC_FAMILY = "r8a77965"/g' local.conf
	elif [ "$LSI" = "M3" ] ; then
		echo; echo 'Changing SOC_FAMILY to "r8a7796"'
		cd ${YOCTO_DIR}/build/conf/
		sed -i '/SOC_FAMILY/d' local.conf
		sed -i 's/# H3: r8a7795, M3: r8a7796, M3N: r8a77965/# H3: r8a7795, M3: r8a7796, M3N: r8a77965\n#SOC_FAMILY = "r8a7795"\nSOC_FAMILY = "r8a7796"\n#SOC_FAMILY = "r8a77965"/g' local.conf
	elif [ "$LSI" = "E3" ] ; then
		echo; echo 'Changing SOC_FAMILY to "r8a77990"'
		cd ${YOCTO_DIR}/meta-renesas
		git am ${ROOT_DIR}/src/linux/kernel/0001-Add-safe-rendering-ebisu.patch
		cd ${YOCTO_DIR}/build/conf/
	fi
	sed -i 's/#MACHINE_FEATURES_append = " safe-rendering"/MACHINE_FEATURES_append = " safe-rendering"/g' local.conf
	echo "bitbake core-image-weston";
	cp ${ROOT_DIR}/src/linux/kernel/0001-samples-renesas_taurus_client-Adding-License-for-Ren.patch \
	   ${ROOT_DIR}/src/linux/kernel/0002-samples-taurus-switch-on-the-COMCH-Taurus-service.patch \
	   ${ROOT_DIR}/src/linux/kernel/0003-samples-taurus-Fix-compilation-error-by-adding-void-.patch \
	   ${ROOT_DIR}/src/linux/kernel/taurus.cfg ${YOCTO_DIR}/meta-renesas/meta-rcar-gen3/recipes-kernel/linux/linux-renesas/
	cd ${YOCTO_DIR}/meta-renesas/
	git apply --check ${ROOT_DIR}/src/linux/0001-rcar-gen3-linux-renesas-Add-.cfg-file-and-patches-fo.patch
	if [ $? -eq 0 ]; then
  		git am ${ROOT_DIR}/src/linux/0001-rcar-gen3-linux-renesas-Add-.cfg-file-and-patches-fo.patch
	fi
	cd ${YOCTO_DIR}
	source poky/oe-init-build-env
	umask a+rx
	umask u+rwx
	bitbake core-image-weston
fi

# Sample command: ./build.sh release m3n
if [ "$RELEASE" = true ] ; then
	echo; echo; echo "R-Car CR7 Solution: Release ${LSI} images to prebuilt folder"; echo;
	if [[ -z "${ROOT_DIR}" ]]; then
	ROOT_DIR=${WORK}/..
	fi
	ROOT_DIR=$(readlink -f $ROOT_DIR)
	# Yocto + CR7
	echo; echo "Copy images for Yocto + CR7 for ${LSI} ..."
	mkdir -p ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/rfs
	mkdir -p ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/tftp
	
	cp ${YOCTO_DIR}/build/tmp/deploy/images/salvator-x/*.srec ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto
	rm ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/u-boot-elf-salvator-x.srec
	if [ "$LSI" = "M3" ] ; then
		cp ${YOCTO_DIR}/build/tmp/deploy/images/salvator-x/r8a77961-salvator-xs_cr7_rproc*.dtb ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/tftp
		cp ${YOCTO_DIR}/build/tmp/deploy/images/salvator-x/Image--*.bin ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/tftp
	elif [ "$LSI" = "H3" ] ; then
		cp ${YOCTO_DIR}/build/tmp/deploy/images/salvator-x/r8a7795-salvator-xs_cr7_rproc*.dtb ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/tftp
		cp ${YOCTO_DIR}/build/tmp/deploy/images/salvator-x/Image--*.bin ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/tftp
	elif [ "$LSI" = "M3N" ] ; then
		cp ${YOCTO_DIR}/build/tmp/deploy/images/salvator-x/r8a77965-salvator-xs_cr7_rproc--*.dtb ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/tftp
		cp ${YOCTO_DIR}/build/tmp/deploy/images/salvator-x/Image--*.bin ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/tftp
	fi
	
	if [ -e ${WORK}/output/cr7/all/elf/cr7_rtos_app.out ]
  		then
		echo "Packing of rproc-cr7-fw in rfs ..."; echo;
		cp ${YOCTO_DIR}/build/tmp/deploy/images/salvator-x/*.rootfs.tar.bz2 ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/rfs
		cd ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/rfs
		mkdir -p core-image-weston-${LSI}
		tar -jxf core-image-weston-*.rootfs.tar.bz2 --directory ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/rfs/core-image-weston-${LSI}
		rm core-image-weston-*.rootfs.tar.bz2
		cp ${WORK}/output/cr7/all/elf/cr7_rtos_app.out ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/rfs/core-image-weston-${LSI}/lib/firmware/rproc-cr7-fw
		cd ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/rfs/core-image-weston-${LSI}
		tar -cjf ./../core-image-weston-${LSI}.tar.bz2 ./*
		cd ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/rfs
		rm -rf core-image-weston-${LSI}
	else
		echo "Cannot copy cr7_rtos_app.out: No such file"; echo "build vlib first";
	fi

	cp ${WORK}/output/cr7/${LSI}/srec/*.srec ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto
	cp ${WORK}/output/cr7/all/srec/cr7_rtos_app.srec ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto
	cd ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto
	if [ -e ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/bl2-salvator-x.srec ]
  		then
		mv bl2-salvator-x.srec bl2.srec
	else
		echo"Cannot rename bl2-salvator-x.srec: No such file"; echo"or bl2.srec already exists";
	fi

	if [ -e ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/bl31-salvator-x.srec ]
  		then
		mv bl31-salvator-x.srec bl31.srec
	else
		echo"Cannot rename bl31-salvator-x.srec: No such file"; echo"or bl31.srec already exists";
	fi

	if [ -e ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/tee-salvator-x.srec ]
  		then
		mv tee-salvator-x.srec tee.srec
	else
		echo"Cannot rename tee-salvator-x.srec: No such file"; echo"or tee.srec already exists";
	fi

	if [ -e ${ROOT_DIR}/linux/prebuilt/${LSI}-Yocto/u-boot-elf-*.srec ]
  		then
		mv u-boot-elf-*.srec u-boot-elf.srec
	else
		echo"Cannot rename u-boot-elf-*.srec: No such file"; echo"or u-boot-elf.srec already exists";
	fi
fi

if [ "$CLEAN" = true ] ; then
	rm -rf ${ROOT_DIR}/linux/prebuilt/	
	echo; echo "R-Car CR7 Solution: clean done"; echo;
else
	echo; echo "R-Car CR7 Solution: Succeed in buidling. Check ${WORK}/linux/prebuilt"; echo;
fi
