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
	atf|arm-trusted-firmware)
	TRUSTED_FIRMWARE=true
	shift
	;;
	android_10|android-10|android10)
	ANDROID_10=true
	shift
	;;
	android_11|android-11|android11)
	ANDROID_11=true
	shift
	;;
	android_13|android-13|android13)
	ANDROID_13=true
	shift
	;;
	nopatch)
	NOPATCH=true
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
	all)
	CR7_IPL=true
	VLIB=true
	TRUSTED_FIRMWARE=true
	ANDROID_10=true
	ANDROID_11=true
	ANDROID_13=true
	shift
	;;
	bootloader)
	CR7_IPL=true
	VLIB=true
	TRUSTED_FIRMWARE=true
	shift
	;;
	clean)
	CLEAN=true
	shift
	;;
	modules)
	MODULES=true
	shift
	;;
	help|-h|h|?)
	echo "--- Usage ---"
	echo "$0 help"
	echo "$0 all|bootloader|cr7_ipl|vlib|atf|android_10|android_11|android_13|release [h3|m3|m3n|e3|modules] [board] [trampoline|freertos] [clean]"
	echo "$0 vlib [h3|m3|m3n|e3] [salvatorxs|ebisu|erguotou] [trampoline|freertos] [app_name] [clean]"
	echo "Example:"
	echo "1) $0 all m3            -- Build all for M3"
	echo "2) $0 bootloader m3     -- Build bootloader (cr7_ipl, vlib, atf, optee_os, uboot) for M3"
	echo "3) $0 vlib m3 hello_rgl -- Build vlib fastboot_mono for M3"
	echo "4) $0 android_11 m3      -- Build Android images for M3"
	echo "5) $0 all clean m3      -- Clean alll build for M3"
	echo "6) $0 release m3        -- Release bins and images for M3 to prebuilt folder"
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

# Root directory
cd ..
ROOT_DIR=$(dirname $(readlink -f $0))
cd android/

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

################# Arm Cortex R7 cr7-vlib code ########################
export CROSS_COMPILE="arm-none-eabi-"
export ARCH=arm

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
		cmake --build . --parallel $(nproc) --target ${VLIBAPP}
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
# 1) ./build.sh cr7_ipl m3
# 2) ./build.sh cr7_ipl m3 debugger
if [ "$CR7_IPL" = true ] ; then
	echo; echo; echo "RCar Gen3 CR7 Solution: Building CR7 IPL"
	cd ${WORK}/../../src/loader/src/CortexR7_Loader
	if [ "$CLEAN" = true ] ; then
		make LSI=${LSI} clean
	else
		# Set 'LOG_LEVEL=40' to output debug info and 'LOG_LEVEL=0' to output none
		# Set 'RCAR_LOAD_RTOS_FROM_DEBUGGER=1' to wait for debugger attachment.
		make LSI=${LSI} clean
		if [ "$LSI" = "M3" ] ; then
			#1.3 (DRAM split - 1, DRAM LPDDR4 - 0)
			#make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=0 RCAR_DRAM_LPDDR4_MEMCONF=0 LOG_LEVEL=0 RCAR_CA5x_OS=2
			#3.0
			make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=2 RCAR_DRAM_LPDDR4_MEMCONF=1 LOG_LEVEL=20 RCAR_CA5x_OS=2
		elif [ "$LSI" = "M3N" ] ; then
			#1.1 (DRAM split - 3, LPDDR4 - 2 (for M3N 4GB))
			CROSS_COMPILE=arm-eabi- make LSI=${LSI} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=3 RCAR_DRAM_LPDDR4_MEMCONF=2 RCAR_CA5x_OS=2
			#1.1 (DRAM split - 3, LPDDR4 - 1 (for M3N 2GB))
			#make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=3 RCAR_DRAM_LPDDR4_MEMCONF=1 LOG_LEVEL=20 RCAR_CA5x_OS=2
		elif [ "$LSI" = "H3" ] ; then
			make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=1 RCAR_DRAM_SPLIT=1 RCAR_DRAM_LPDDR4_MEMCONF=1 LOG_LEVEL=20 RCAR_CA5x_OS=2
		elif [ "$LSI" = "E3" ] ; then
			# Ebisu-4D
			make LSI=${LSI} RCAR_LOAD_RTOS_FROM_DEBUGGER=${CR7_IPL_DBG} RCAR_LIFEC_NON_SECURE_MASTER=0 RCAR_SECURE_BOOT=0 RCAR_KICK_MAIN_CPU=2 RCAR_SA0_SIZE=0 RCAR_DRAM_DDR3L_MEMCONF=1 RCAR_DRAM_DDR3L_MEMDUAL=1 LOG_LEVEL=20 RCAR_CA5x_OS=2
		fi

		cp bootparam_sa0.bin cert_header_sa3.bin cr7_loader.bin ${WORK}/output/cr7/${LSI}/bin
		cp bootparam_sa0.srec cert_header_sa3.srec cr7_loader.srec ${WORK}/output/cr7/${LSI}/srec
		cp bootparam_sa0.elf cert_header_sa3.elf cr7_loader.elf ${WORK}/output/cr7/${LSI}/elf
	fi
	cd ..
fi

# Sample command: ./build.sh android_10 m3
if [ "$ANDROID_10" = true ] ; then
	echo; echo; echo "Renesas R-Car CR7 Solution: Building Android 10 for ${LSI}"
	unset CROSS_COMPILE
	unset ARCH

	if [[ -z "${MYDROID_10}" ]]; then
		echo "Warning: MYDROID_10 is not specified! Skip building!"
	else
		if [ "$LSI" = "M3" ] ; then
			export TARGET_BOARD_PLATFORM=r8a7796
		elif [ "$LSI" = "H3" ] ; then
			export TARGET_BOARD_PLATFORM=r8a7795
			export H3_OPTION=8GB
		elif [ "$LSI" = "M3N" ] ; then
			export TARGET_BOARD_PLATFORM=r8a77965
		else
			unset TARGET_BOARD_PLATFORM
			echo "Error: Wrong SoC type! Android BSP supports R-Car H3, M3, or M3N only!"
			exit
		fi

		# Reserve original source file before patching
		if [[ ! -d "${WORK}/backup/android-10" ]]; then
			mkdir -p ${WORK}/backup/android-10
			cp ${MYDROID_10}/device/renesas/common/init/init.common.rc ${WORK}/backup/android-10/
			cp ${MYDROID_10}/device/renesas/salvator/init/ueventd.salvator.rc ${WORK}/backup/android-10/
			cp ${MYDROID_10}/device/renesas/common/DeviceCommon.mk ${WORK}/backup/android-10/
			cp ${MYDROID_10}/device/renesas/common/ModulesCommon.mk ${WORK}/backup/android-10/
			cp -dr ${MYDROID_10}/vendor/renesas/hal/evs ${WORK}/backup/android-10/
			cp -dr ${MYDROID_10}/hardware/renesas/modules/uvcs ${WORK}/backup/android-10/
		fi

		# Patch ...
		if [[ -z "${NOPATCH}" ]]; then
			echo; echo "Copy patching files ..."
			cp ${ROOT_DIR}/src/android-10/init.common.rc ${MYDROID_10}/device/renesas/common/init/
			cp ${ROOT_DIR}/src/android-10/ueventd.salvator.rc ${MYDROID_10}/device/renesas/salvator/init/
			cp ${ROOT_DIR}/src/android-10/*.mk ${MYDROID_10}/device/renesas/common/
			cp ${ROOT_DIR}/src/android-10/remoteproc/ ${MYDROID_10}/vendor/renesas/firmware/ -rf
			cp -dr ${ROOT_DIR}/src/android-10/evs/* ${MYDROID_10}/vendor/renesas/hal/evs/
			cp -dr ${ROOT_DIR}/src/android-10/uvcs/* ${MYDROID_10}/hardware/renesas/modules/uvcs/
		fi
		cp ${WORK}/output/cr7/all/elf/cr7_rtos_app.out ${MYDROID_10}/vendor/renesas/firmware/remoteproc/rproc-cr7-fw
		if [ $? -ne 0 ]; then
			echo "Copy cr7 firmware error! Build vlib first!"
			exit
		fi

		# Build Android 10
		export NUM_JOBS=$(($(grep ^processor /proc/cpuinfo | wc -l)*2))
		cd ${MYDROID_10}
		source build/envsetup.sh
		lunch salvator-userdebug
		export BUILD_BOOTLOADERS=true
		export BUILD_BOOTLOADERS_SREC=true
		
		# EMMC_BOOT_P2
		# 0:Disable boot from eMMC partition 2 for CR7 solution
		# 1:Enable boot from eMMC partition 2 for CR7 solution
		# 	also, should set
		# 		RCAR_SA6_TYPE=1
		#		RCAR_BOOT_EMMC=1
		export EMMC_BOOT_P2=0
		export RCAR_SA6_TYPE=0
		export RCAR_BOOT_EMMC=0
		export TARGET_RVGC_ENABLED=true
		if [ "$CLEAN" = true ] ; then
			echo; echo "Remove entire build directory ..."
			make clean
		else
			make -j${NUM_JOBS}

			# Copy images
			echo; echo "Copy images ..."
			mkdir -p ${WORK}/output/android-10/${LSI}/ipl-srec
			mkdir -p ${WORK}/output/android-10/${LSI}/ipl-bin
			cd ${WORK}/output/android-10/${LSI}/
			cp ${MYDROID_10}/out/target/product/salvator/boot.img ./
			cp ${MYDROID_10}/out/target/product/salvator/dtb.img ./
			cp ${MYDROID_10}/out/target/product/salvator/dtbo.img ./
			cp ${MYDROID_10}/out/target/product/salvator/vbmeta.img ./
			cp ${MYDROID_10}/out/target/product/salvator/system.img ./
			cp ${MYDROID_10}/out/target/product/salvator/vendor.img ./
			cp ${MYDROID_10}/out/target/product/salvator/product.img ./
			cp ${MYDROID_10}/out/target/product/salvator/bootloader.img ./
			cp ${MYDROID_10}/out/target/product/salvator/odm.img ./
			cp ${MYDROID_10}/out/target/product/salvator/ramdisk.img ./
			cp ${MYDROID_10}/out/target/product/salvator/ramdisk-debug.img ./
			cp ${MYDROID_10}/out/target/product/salvator/boot-debug.img ./
			cp ${MYDROID_10}/out/target/product/salvator/super.img ./
			cp ${MYDROID_10}/out/target/product/salvator/super_empty.img ./
			cp ${MYDROID_10}/out/target/product/salvator/*.srec ./ipl-srec/
			cp ${MYDROID_10}/vendor/renesas/utils/fastboot/fastboot.sh ./
			cp ${MYDROID_10}/vendor/renesas/utils/fastboot/fastboot_functions.sh ./
			cp ${MYDROID_10}/out/host/linux-x86/bin/adb ./
			cp ${MYDROID_10}/out/host/linux-x86/bin/mke2fs ./
			cp ${MYDROID_10}/out/host/linux-x86/bin/fastboot ./
			cp ${MYDROID_10}/out/target/product/salvator/obj/IPL_OBJ/rcar/release/*.bin ./ipl-bin/
			cp ${MYDROID_10}/out/target/product/salvator/obj/IPL_SA_HF_OBJ/tools/dummy_create/*.bin ./ipl-bin/
			cp ${MYDROID_10}/out/target/product/salvator/obj/OPTEE_OBJ/core/tee.bin ./ipl-bin/
			cp ${MYDROID_10}/out/target/product/salvator/obj/UBOOT_OBJ/u-boot.bin ./ipl-bin/
		fi
	fi
fi

# Sample command: ./build.sh android_11 m3n
if [ "$ANDROID_11" = true ] ; then
	# If MYDROID_11 is setup, and mydroid already exist, we
	if [ -v MYDROID_11 ]; then
		if [ ! -d "$MYDROID_11" ]; then
			echo "MYDROID_11 is set but mydroid folder is missing."
			echo "Please unset MYDROID_11 or check the configuration in setup-env.sh"
			exit 1
		fi
	else
		if [ ! -e "${ANDROID11_PKG_DIR}/${ANDROID11_PKG_NAME}" ]; then
			echo "$ANDROID11_PKG_NAME does not exist. Exiting script."
			exit 1
		fi
		# Since the MYDROID_11 isn't defined by the caller let's try to figure out it based on the package path
		export MYDROID_11="${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3/OSS_Package/Gen3_Android_v11_1.1.3/RENESAS_RCH3M3M3N_Android_11_ReleaseNote_2021_03E/mydroid"
		# Check if the mydroid really does exist in the expected package directory
		if [ ! -d "${MYDROID_11}" ]; then
			echo "mydroid not found, building Android 11..."
			
			unzip ${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3.zip -d ${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3
			unzip ${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3/OSS_Package/Gen3_Android_v11_1.1.3.zip -d ${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3/OSS_Package
			unzip ${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3/OSS_Package/Gen3_Android_v11_1.1.3/RENESAS_RCH3M3M3N_Android_11_ReleaseNote_2021_03E.zip -d ${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3/OSS_Package/Gen3_Android_v11_1.1.3
			cd ${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3
			curl http://commondatastorage.googleapis.com/git-repo-downloads/repo > repo
			chmod +x repo
			export PATH=$(pwd):${PATH}
			cd ${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3/OSS_Package/Gen3_Android_v11_1.1.3/RENESAS_RCH3M3M3N_Android_11_ReleaseNote_2021_03E
			export PKGS_DIR=${ANDROID11_PKG_DIR}/Gen3_Android_v11_1.1.3/Software/SV00_Android_11_1.1.3_Software_001
			unzip ${PKGS_DIR}.zip -d ${PKGS_DIR}
			mkdir pkgs_dir
			cp -r ${PKGS_DIR}/proprietary/pkgs_dir/* pkgs_dir
			export workspace=$(pwd)
			chmod +x walkthrough.sh

			./walkthrough.sh ${LSI}

			cd ${ROOT_DIR}/android

			echo; echo; echo "Applying CR7 patches..."
			./setup-repos.sh
		fi
	fi
	
	echo; echo; echo "Renesas R-Car CR7 Solution: Building Android 11 for ${LSI}"
	unset CROSS_COMPILE
	unset ARCH

	if [[ -z "${MYDROID_11}" ]]; then
		echo "Warning: MYDROID_11 is not specified! Skip building!"
	else
		if [ "$LSI" = "M3" ] ; then
			export TARGET_BOARD_PLATFORM=r8a7796
		elif [ "$LSI" = "H3" ] ; then
			export TARGET_BOARD_PLATFORM=r8a7795
			export H3_OPTION=8GB
		elif [ "$LSI" = "M3N" ] ; then
			export TARGET_BOARD_PLATFORM=r8a77965
		else
			unset TARGET_BOARD_PLATFORM
			echo "Error: Wrong SoC type! Android BSP supports R-Car H3, M3, or M3N only!"
			exit
		fi

		cp ${WORK}/output/cr7/all/elf/cr7_rtos_app.out ${MYDROID_11}/vendor/renesas/firmware/remoteproc/rproc-cr7-fw
		if [ $? -ne 0 ]; then
			echo "Copy cr7 firmware error! Build vlib first!"
			exit
		fi

		# Build Android 11
		cd ${MYDROID_11}
		source build/envsetup.sh
		lunch salvator-userdebug
		export BUILD_BOOTLOADERS=true
		export BUILD_BOOTLOADERS_SREC=true

		# EMMC_BOOT_P2
		# 0:Disable boot from eMMC partition 2 for CR7 solution
		# 1:Enable boot from eMMC partition 2 for CR7 solution
		# 	also, should set
		# 		RCAR_SA6_TYPE=1
		#		RCAR_BOOT_EMMC=1
		export EMMC_BOOT_P2=0
		export RCAR_SA6_TYPE=0
		export RCAR_BOOT_EMMC=0
		export TARGET_RVGC_ENABLED=true
		if [ "$CLEAN" = true ] ; then
			echo; echo "Remove build artifacts ..."
			make installclean
		else
			make

			# Copy images
			echo; echo "Copy images ..."
			mkdir -p ${WORK}/output/android-11/${LSI}/ipl-srec
			mkdir -p ${WORK}/output/android-11/${LSI}/ipl-bin
			cd ${WORK}/output/android-11/${LSI}/
			cp ${MYDROID_11}/out/target/product/salvator/boot.img ./
			cp ${MYDROID_11}/out/target/product/salvator/dtb.img ./
			cp ${MYDROID_11}/out/target/product/salvator/dtbo.img ./
			cp ${MYDROID_11}/out/target/product/salvator/vbmeta.img ./
			cp ${MYDROID_11}/out/target/product/salvator/system.img ./
			cp ${MYDROID_11}/out/target/product/salvator/vendor.img ./
			cp ${MYDROID_11}/out/target/product/salvator/vendor_boot.img ./
			cp ${MYDROID_11}/out/target/product/salvator/product.img ./
			cp ${MYDROID_11}/out/target/product/salvator/bootloader.img ./
			cp ${MYDROID_11}/out/target/product/salvator/odm.img ./
			cp ${MYDROID_11}/out/target/product/salvator/ramdisk.img ./
			cp ${MYDROID_11}/out/target/product/salvator/ramdisk-debug.img ./
			cp ${MYDROID_11}/out/target/product/salvator/boot-debug.img ./
			cp ${MYDROID_11}/out/target/product/salvator/super.img ./
			cp ${MYDROID_11}/out/target/product/salvator/super_empty.img ./
			cp ${MYDROID_11}/out/target/product/salvator/*.srec ./ipl-srec/
			cp ${MYDROID_11}/vendor/renesas/utils/fastboot/fastboot.sh ./
			cp ${MYDROID_11}/vendor/renesas/utils/fastboot/fastboot_functions.sh ./
			cp ${MYDROID_11}/out/host/linux-x86/bin/adb ./
			cp ${MYDROID_11}/out/host/linux-x86/bin/mke2fs ./
			cp ${MYDROID_11}/out/host/linux-x86/bin/fastboot ./
			cp ${MYDROID_11}/out/target/product/salvator/obj/IPL_OBJ/rcar/release/*.bin ./ipl-bin/
			cp ${MYDROID_11}/out/target/product/salvator/obj/IPL_SA_HF_OBJ/tools/dummy_create/*.bin ./ipl-bin/
			cp ${MYDROID_11}/out/target/product/salvator/obj/OPTEE_OBJ/core/tee.bin ./ipl-bin/
			cp ${MYDROID_11}/out/target/product/salvator/obj/UBOOT_OBJ/u-boot.bin ./ipl-bin/
		fi
	fi
fi

if [ "$ANDROID_13" = true ] ; then
	echo; echo; echo "Renesas R-Car CR7 Solution: Building Android 13 for ${LSI}"
	unset CROSS_COMPILE
	unset ARCH

	if [[ -z "${MYDROID_13}" ]]; then
		echo "Warning: MYDROID_13 is not specified! Skip building!"
	else
		if [ "$LSI" = "M3" ] ; then
			export TARGET_BOARD_PLATFORM=r8a7796
		elif [ "$LSI" = "H3" ] ; then
			export TARGET_BOARD_PLATFORM=r8a7795
			export H3_OPTION=8GB
		elif [ "$LSI" = "M3N" ] ; then
			export TARGET_BOARD_PLATFORM=r8a77965
		else
			unset TARGET_BOARD_PLATFORM
			echo "Error: Wrong SoC type! Android BSP supports R-Car H3, M3, or M3N only!"
			exit
		fi

		cp -av ${WORK}/output/cr7/all/elf/cr7_rtos_app.out ${MYDROID_13}/vendor/renesas/firmware/remoteproc/rproc-cr7-fw
		if [ $? -ne 0 ]; then
			echo "Copy cr7 firmware error! Build vlib first!"
			exit
		fi

		# Build Android 13
		pushd ${MYDROID_13}
		source build/envsetup.sh
		lunch salvator-userdebug
		export BUILD_BOOTLOADERS=true
		export BUILD_BOOTLOADERS_SREC=true

		# EMMC_BOOT_P2
		# 0:Disable boot from eMMC partition 2 for CR7 solution
		# 1:Enable boot from eMMC partition 2 for CR7 solution
		# 	also, should set
		# 		RCAR_SA6_TYPE=1
		#		RCAR_BOOT_EMMC=1
		export EMMC_BOOT_P2=0
		export RCAR_SA6_TYPE=0
		export RCAR_BOOT_EMMC=0
		export TARGET_RVGC_ENABLED=true
		if [ "$CLEAN" = true ] ; then
			echo; echo "Remove build artifacts ..."
			make installclean
		else
			make

			# Copy images
			echo; echo "Copy images ..."
			mkdir -p ${WORK}/output/android-13/${LSI}/ipl-srec
			mkdir -p ${WORK}/output/android-13/${LSI}/ipl-bin
			pushd ${WORK}/output/android-13/${LSI}/
			cp ${MYDROID_13}/out/target/product/salvator/boot.img ./
			cp ${MYDROID_13}/out/target/product/salvator/dtb.img ./
			cp ${MYDROID_13}/out/target/product/salvator/dtbo.img ./
			cp ${MYDROID_13}/out/target/product/salvator/vbmeta.img ./
			cp ${MYDROID_13}/out/target/product/salvator/system.img ./
			cp ${MYDROID_13}/out/target/product/salvator/vendor.img ./
			cp ${MYDROID_13}/out/target/product/salvator/vendor_boot.img ./
			cp ${MYDROID_13}/out/target/product/salvator/product.img ./
			cp ${MYDROID_13}/out/target/product/salvator/bootloader.img ./
			cp ${MYDROID_13}/out/target/product/salvator/odm.img ./
			cp ${MYDROID_13}/out/target/product/salvator/ramdisk.img ./
			cp ${MYDROID_13}/out/target/product/salvator/super.img ./
			cp ${MYDROID_13}/out/target/product/salvator/super_empty.img ./
			cp ${MYDROID_13}/out/target/product/salvator/*.srec ./ipl-srec/
			cp ${MYDROID_13}/vendor/renesas/utils/fastboot/fastboot.sh ./
			cp ${MYDROID_13}/vendor/renesas/utils/fastboot/fastboot_functions.sh ./
			cp ${MYDROID_13}/out/host/linux-x86/bin/adb ./
			cp ${MYDROID_13}/out/host/linux-x86/bin/mke2fs ./
			cp ${MYDROID_13}/out/host/linux-x86/bin/fastboot ./
			cp ${MYDROID_13}/out/target/product/salvator/obj/IPL_OBJ/rcar/release/*.bin ./ipl-bin/
			cp ${MYDROID_13}/out/target/product/salvator/obj/IPL_SA_HF_OBJ/tools/renesas/rcar_layout_create/*.bin ./ipl-bin/
			cp ${MYDROID_13}/out/target/product/salvator/obj/OPTEE_OBJ/core/tee.bin ./ipl-bin/
			cp ${MYDROID_13}/out/target/product/salvator/obj/UBOOT_OBJ/u-boot.bin ./ipl-bin/
			popd
		fi
		popd
	fi
fi

# Sample command: ./build.sh release m3
if [ "$RELEASE" = true ] ; then
	echo; echo; echo "R-Car CR7 Solution: Release ${LSI} images to prebuilt folder"; echo;

	# Android 10 + CR7
	if [[ -n "${MYDROID_10}" ]] && [[ "$LSI" != "E3" ]]; then
		echo; echo "Copy images for Android 10 + CR7 for ${LSI} ..."
		mkdir -p ${ROOT_DIR}/android/prebuilt/${LSI}-Android-10/bootloaders
		cp -r ${WORK}/output/android-10/${LSI}/ipl-srec/*.srec ${ROOT_DIR}/android/prebuilt/${LSI}-Android-10/bootloaders
		cp -r ${WORK}/output/cr7/${LSI}/srec/*.srec ${ROOT_DIR}/android/prebuilt/${LSI}-Android-10/bootloaders
		cp -r ${WORK}/output/android-10/${LSI}/* ${ROOT_DIR}/android/prebuilt/${LSI}-Android-10
		cp -r ${WORK}/output/cr7/all/srec/*.srec ${ROOT_DIR}/android/prebuilt/${LSI}-Android-10
		# Fix 'reboot fastboot' bug of fastboot.sh and disable flashing bl2.bin
		cp ${ROOT_DIR}/src/android-10/fastboot.sh ${ROOT_DIR}/android/prebuilt/${LSI}-Android-10
	fi

	# Android 11 + CR7
	if [[ -n "${MYDROID_11}" ]] && [[ "$LSI" != "E3" ]]; then
		echo; echo "Copy images for Android 11 + CR7 for ${LSI} ..."
		mkdir -p ${ROOT_DIR}/android/prebuilt/${LSI}-Android-11
		cp -r ${WORK}/output/android-11/${LSI}/ipl-srec/*.srec ${ROOT_DIR}/android/prebuilt/${LSI}-Android-11
		cp ${WORK}/output/android-11/${LSI}/ipl-bin/bl2.bin ${ROOT_DIR}/android/prebuilt/${LSI}-Android-11
		cp -r ${WORK}/output/android-11/${LSI}/* ${ROOT_DIR}/android/prebuilt/${LSI}-Android-11
		cp -r ${WORK}/output/cr7/${LSI}/srec/*.srec ${ROOT_DIR}/android/prebuilt/${LSI}-Android-11
		cp -r ${WORK}/output/cr7/all/srec/*.srec ${ROOT_DIR}/android/prebuilt/${LSI}-Android-11
	fi

	# Android 13 + CR7
	if [[ -n "${MYDROID_13}" ]] && [[ "$LSI" != "E3" ]]; then
		echo; echo "Copy images for Android 13 + CR7 for ${LSI} ..."
		mkdir -p ${ROOT_DIR}/android/prebuilt/${LSI}-Android-13
		cp -r ${WORK}/output/android-13/${LSI}/ipl-srec/*.srec ${ROOT_DIR}/android/prebuilt/${LSI}-Android-13
		cp ${WORK}/output/android-13/${LSI}/ipl-bin/bl2.bin ${ROOT_DIR}/android/prebuilt/${LSI}-Android-13
		cp -r ${WORK}/output/android-13/${LSI}/* ${ROOT_DIR}/android/prebuilt/${LSI}-Android-13
		cp -r ${WORK}/output/cr7/${LSI}/srec/*.srec ${ROOT_DIR}/android/prebuilt/${LSI}-Android-13
		cp -r ${WORK}/output/cr7/all/srec/*.srec ${ROOT_DIR}/android/prebuilt/${LSI}-Android-13
	fi
fi

if [ "$CLEAN" = true ] ; then
	echo; echo; echo "R-Car CR7 Solution: clean done"; echo;
else
	echo; echo; echo "R-Car CR7 Solution: Succeed in buidling. Check ${WORK}/output"; echo;
fi
