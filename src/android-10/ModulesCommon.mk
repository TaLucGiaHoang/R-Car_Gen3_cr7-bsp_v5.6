#
# Copyright (C) 2019 GlobalLogic
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

ifeq ($(KERNEL_MODULES_OUT),)
$(error "KERNEL_MODULES_OUT is not set")
endif

ifeq ($(PRODUCT_OUT),)
$(error "PRODUCT_OUT is not set")
endif

BOARD_VENDOR_KERNEL_MODULES += \
	$(KERNEL_MODULES_OUT)/pvrsrvkm.ko \
	$(KERNEL_MODULES_OUT)/vspm.ko \
	$(KERNEL_MODULES_OUT)/vspm_if.ko \
	$(KERNEL_MODULES_OUT)/mmngr.ko \
	$(KERNEL_MODULES_OUT)/mmngrbuf.ko \
	$(KERNEL_MODULES_OUT)/uvcs_drv.ko \
	$(KERNEL_MODULES_OUT)/qos.ko

ifeq ($(ENABLE_ADSP),true)
	BOARD_VENDOR_KERNEL_MODULES += \
		$(KERNEL_MODULES_OUT)/xtensa-hifi.ko
endif

BOARD_VENDOR_KERNEL_MODULES += \
	$(KERNEL_MODULES_OUT)/extcon-usb-gpio.ko \
	$(KERNEL_MODULES_OUT)/phy-rcar-gen3-usb2.ko \
	$(KERNEL_MODULES_OUT)/renesas_usbhs.ko \
	$(KERNEL_MODULES_OUT)/ehci-hcd.ko \
	$(KERNEL_MODULES_OUT)/ehci-platform.ko \
	$(KERNEL_MODULES_OUT)/ohci-hcd.ko \
	$(KERNEL_MODULES_OUT)/ohci-platform.ko \
	$(KERNEL_MODULES_OUT)/xhci-hcd.ko \
	$(KERNEL_MODULES_OUT)/xhci-plat-hcd.ko \
	$(KERNEL_MODULES_OUT)/usb-storage.ko

BOARD_VENDOR_KERNEL_MODULES += \
	$(KERNEL_MODULES_OUT)/micrel.ko

BOARD_VENDOR_KERNEL_MODULES += \
	$(KERNEL_MODULES_OUT)/rcar_cr7_remoteproc.ko \
	$(KERNEL_MODULES_OUT)/rpmsg_core.ko \
	$(KERNEL_MODULES_OUT)/virtio_rpmsg_bus.ko \
	$(KERNEL_MODULES_OUT)/rcar-rvgc-drm.ko \
	$(KERNEL_MODULES_OUT)/rcar-vivid.ko

BOARD_VENDOR_KERNEL_MODULES += \
	$(KERNEL_MODULES_OUT)/ravb_proc.ko  \
	$(KERNEL_MODULES_OUT)/ravb_streaming.ko \
	$(KERNEL_MODULES_OUT)/mch_core.ko \
	$(KERNEL_MODULES_OUT)/mse_core.ko \
	$(KERNEL_MODULES_OUT)/mse_adapter_alsa.ko \
	$(KERNEL_MODULES_OUT)/mse_adapter_v4l2.ko \
	$(KERNEL_MODULES_OUT)/mse_adapter_eavb.ko \
	$(KERNEL_MODULES_OUT)/mse_adapter_mch.ko \
	$(KERNEL_MODULES_OUT)/8812au.ko

BOARD_RECOVERY_KERNEL_MODULES += \
	$(KERNEL_MODULES_OUT)/extcon-usb-gpio.ko \
	$(KERNEL_MODULES_OUT)/phy-rcar-gen3-usb2.ko \
	$(KERNEL_MODULES_OUT)/renesas_usbhs.ko

ROGUE_KM_SRC            := hardware/renesas/modules/gfx/build/linux/$(TARGET_BOARD_PLATFORM)_android
ROGUE_KM_OUT            := $(PRODUCT_OUT)/obj/ROGUE_KM_OBJ
ROGUE_KM_OUT_ABS        := $(abspath $(ROGUE_KM_OUT))
ROGUE_KM                := $(ROGUE_KM_OUT)/target_aarch64/kbuild/pvrsrvkm.ko

VSPM_KM_SRC             := hardware/renesas/modules/vspm/vspm-module
VSPM_KM_OUT             := $(PRODUCT_OUT)/obj/VSPM_KM_OBJ
VSPM_KM_OUT_ABS         := $(abspath $(VSPM_KM_OUT))
VSPM_KM                 := $(VSPM_KM_OUT)/vspm-module/files/vspm/drv/vspm.ko

VSPMIF_KM_SRC           := hardware/renesas/modules/vspmif/vspm_if-module
VSPMIF_KM_OUT           := $(PRODUCT_OUT)/obj/VSPMIF_KM_OBJ
VSPMIF_KM_OUT_ABS       := $(abspath $(VSPMIF_KM_OUT))
VSPMIF_KM               := $(VSPMIF_KM_OUT)/vspm_if-module/files/vspm_if/drv/vspm_if.ko

MMNGR_KM_SRC            := hardware/renesas/modules/mmngr/mmngr_drv/mmngr/mmngr-module/files/mmngr
MMNGR_KM_OUT            := $(PRODUCT_OUT)/obj/MMNGR_KM_OBJ
MMNGR_KM_OUT_ABS        := $(abspath $(MMNGR_KM_OUT))
MMNGR_KM                := $(MMNGR_KM_OUT)/mmngr/drv/mmngr.ko

MMNGRBUF_KM_SRC         := hardware/renesas/modules/mmngr/mmngr_drv/mmngrbuf/mmngrbuf-module/files/mmngrbuf
MMNGRBUF_KM_OUT         := $(PRODUCT_OUT)/obj/MMNGRBUF_KM_OBJ
MMNGRBUF_KM_OUT_ABS     := $(abspath $(MMNGRBUF_KM_OUT))
MMNGRBUF_KM             := $(MMNGRBUF_KM_OUT)/mmngrbuf/drv/mmngrbuf.ko

UVCS_KM_SRC             := hardware/renesas/modules/uvcs
UVCS_KM_OUT             := $(PRODUCT_OUT)/obj/UVCS_KM_OBJ
UVCS_KM_OUT_ABS         := $(abspath $(UVCS_KM_OUT))
UVCS_KM                 := $(UVCS_KM_OUT)/src/makefile/uvcs_drv.ko

QOS_KM_SRC              := hardware/renesas/modules/qos/qos-module/files/qos/drv
QOS_KM_OUT              := $(PRODUCT_OUT)/obj/QOS_KM_OBJ
QOS_KM_OUT_ABS          := $(abspath $(QOS_KM_OUT))
QOS_KM                  := $(QOS_KM_OUT)/qos.ko

ETHAVB_STR_KM_SRC       := hardware/renesas/modules/avb-streaming
ETHAVB_STR_KM_OUT       := $(PRODUCT_OUT)/obj/ETHAVB_STREAMING_KM_OBJ
ETHAVB_STR_KM_OUT_ABS   := $(abspath $(ETHAVB_STR_KM_OUT))
ETHAVB_STR_KM           := $(ETHAVB_STR_KM_OUT)/ravb_streaming.ko

ETHAVB_MCH_KM_SRC       := hardware/renesas/modules/avb-mch
ETHAVB_MCH_KM_OUT       := $(PRODUCT_OUT)/obj/ETHAVB_MCH_KM_OBJ
ETHAVB_MCH_KM_OUT_ABS   := $(abspath $(ETHAVB_MCH_KM_OUT))
ETHAVB_MCH_KM           := $(ETHAVB_MCH_KM_OUT)/mch_core.ko

ETHAVB_MSE_KM_SRC       := hardware/renesas/modules/avb-mse
ETHAVB_MSE_KM_OUT       := $(PRODUCT_OUT)/obj/ETHAVB_MSE_KM_OBJ
ETHAVB_MSE_KM_OUT_ABS   := $(abspath $(ETHAVB_MSE_KM_OUT))
ETHAVB_MSE_KM           := $(ETHAVB_MSE_KM_OUT)/mse_core.ko
ETHAVB_MSE_KM_INCLUDES  := $(abspath $(ETHAVB_STR_KM_SRC)) $(abspath $(ETHAVB_MCH_KM_SRC))

ADSP_KM_SRC             := hardware/renesas/modules/adsp-s492c
ADSP_KM_OUT             := $(PRODUCT_OUT)/obj/ADSP_KM_OBJ
ADSP_KM_OUT_ABS         := $(abspath $(ADSP_KM_OUT))
ADSP_KM                 := $(ADSP_KM_OUT)/xtensa-hifi.ko

WLAN_KM_SRC             := hardware/realtek/wlan/rtl8812au_km
WLAN_KM_OUT             := $(PRODUCT_OUT)/obj/WLAN_KM_OBJ
WLAN_KM_OUT_ABS         := $(abspath $(WLAN_KM_OUT))
WLAN_KM                 := $(WLAN_KM_OUT)/8812au.ko

# rgx module
$(ROGUE_KM):
	$(ANDROID_MAKE) -C $(ROGUE_KM_SRC) KERNELDIR=$(KERNEL_OUT_ABS) \
		ANDROID_ROOT=$(abspath $(TOP)) $(KERNEL_COMPILE_FLAGS) \
		OUT=$(ROGUE_KM_OUT_ABS) TARGET_DEVICE=$(TARGET_DEVICE)
	cp $(ROGUE_KM) $(KERNEL_MODULES_OUT)/

# vspm module
$(VSPM_KM):
	mkdir -p $(VSPM_KM_OUT_ABS)
	cp -pR $(VSPM_KM_SRC) $(VSPM_KM_OUT_ABS)
	$(ANDROID_MAKE) -C $(VSPM_KM_OUT_ABS)/vspm-module/files/vspm/drv \
		$(KERNEL_COMPILE_FLAGS) KERNELDIR=$(KERNEL_OUT_ABS) \
		PWD=$(VSPM_KM_OUT_ABS)/vspm-module/files/vspm/drv/
	cp $(VSPM_KM) $(KERNEL_MODULES_OUT)/

# vspm-if module
$(VSPMIF_KM): $(VSPM_KM)
	mkdir -p $(VSPMIF_KM_OUT_ABS)
	cp -pR $(VSPMIF_KM_SRC) $(VSPMIF_KM_OUT_ABS)
	$(ANDROID_MAKE) -C $(VSPMIF_KM_OUT_ABS)/vspm_if-module/files/vspm_if/drv/ \
		$(KERNEL_COMPILE_FLAGS) CP=cp KERNELDIR=$(KERNEL_OUT_ABS) \
		PWD=$(VSPMIF_KM_OUT_ABS)/vspm_if-module/files/vspm_if/drv/ \
		KBUILD_EXTRA_SYMBOLS="$(VSPM_KM_OUT_ABS)/vspm-module/files/vspm/drv/Module.symvers"
	cp $(VSPMIF_KM) $(KERNEL_MODULES_OUT)/

# mmngr module
$(MMNGR_KM):
	mkdir -p $(MMNGR_KM_OUT_ABS)
	cp -pR $(MMNGR_KM_SRC) $(MMNGR_KM_OUT_ABS)
	$(ANDROID_MAKE) -C $(MMNGR_KM_OUT_ABS)/mmngr/drv \
		$(KERNEL_COMPILE_FLAGS) KERNELDIR=$(KERNEL_OUT_ABS) \
		PWD=$(MMNGR_KM_OUT_ABS)/mmngr/drv \
		MMNGR_CONFIG=MMNGR_SALVATORX \
		MMNGR_SSP_CONFIG="MMNGR_SSP_DISABLE" \
		MMNGR_IPMMU_MMU_CONFIG="IPMMU_MMU_DISABLE"
	cp $(MMNGR_KM) $(KERNEL_MODULES_OUT)/

# mmngrbuf module
$(MMNGRBUF_KM):
	mkdir -p $(MMNGRBUF_KM_OUT_ABS)
	cp -pR $(MMNGRBUF_KM_SRC) $(MMNGRBUF_KM_OUT_ABS)
	$(ANDROID_MAKE) -C $(MMNGRBUF_KM_OUT_ABS)/mmngrbuf/drv \
		$(KERNEL_COMPILE_FLAGS) KERNELDIR=$(KERNEL_OUT_ABS) \
		PWD=$(MMNGRBUF_KM_OUT_ABS)/mmngrbuf/drv \
		MMNGR_CONFIG=MMNGR_SALVATORX \
		MMNGR_SSP_CONFIG="MMNGR_SSP_DISABLE" \
		MMNGR_IPMMU_MMU_CONFIG="IPMMU_MMU_DISABLE"
	cp $(MMNGRBUF_KM) $(KERNEL_MODULES_OUT)/

# UVCS module
$(UVCS_KM):
	mkdir -p $(UVCS_KM_OUT_ABS)
	cp -pR $(UVCS_KM_SRC)/include $(UVCS_KM_OUT_ABS)/
	cp -pR $(UVCS_KM_SRC)/src $(UVCS_KM_OUT_ABS)/
	TOP=. $(ANDROID_MAKE) -C $(UVCS_KM_OUT_ABS)/src/makefile \
	    $(KERNEL_COMPILE_FLAGS) KERNELDIR=$(KERNEL_OUT_ABS)
	cp $(UVCS_KM) $(KERNEL_MODULES_OUT)/

# QoS module
$(QOS_KM):
	mkdir -p $(QOS_KM_OUT_ABS)
	cp -pR $(QOS_KM_SRC)/* $(QOS_KM_OUT_ABS)/
	$(ANDROID_MAKE) -C $(QOS_KM_OUT_ABS) $(KERNEL_COMPILE_FLAGS) \
		KERNELDIR=$(KERNEL_OUT_ABS)
	cp $(QOS_KM) $(KERNEL_MODULES_OUT)/

# Ethernet avb-streaming module
$(ETHAVB_STR_KM):
	mkdir -p $(ETHAVB_STR_KM_OUT_ABS)
	cp -pR $(ETHAVB_STR_KM_SRC)/* $(ETHAVB_STR_KM_OUT_ABS)/
	$(ANDROID_MAKE) -C $(KERNEL_OUT_ABS) $(KERNEL_COMPILE_FLAGS) \
		KERNELDIR=$(KERNEL_OUT_ABS) M=$(ETHAVB_STR_KM_OUT_ABS)
	cp $(ETHAVB_STR_KM_OUT_ABS)/*.ko $(KERNEL_MODULES_OUT)/

# avb-mch module
$(ETHAVB_MCH_KM):
	mkdir -p $(ETHAVB_MCH_KM_OUT_ABS)
	cp -pR $(ETHAVB_MCH_KM_SRC)/* $(ETHAVB_MCH_KM_OUT_ABS)/
	$(ANDROID_MAKE) -C $(KERNEL_OUT_ABS) $(KERNEL_COMPILE_FLAGS) \
		KERNELDIR=$(KERNEL_OUT_ABS) M=$(ETHAVB_MCH_KM_OUT_ABS)
	cp $(ETHAVB_MCH_KM_OUT_ABS)/*.ko $(KERNEL_MODULES_OUT)/

# avb-mse module
$(ETHAVB_MSE_KM):
	mkdir -p $(ETHAVB_MSE_KM_OUT_ABS)
	cp -pR $(ETHAVB_MSE_KM_SRC)/* $(ETHAVB_MSE_KM_OUT_ABS)/
	$(ANDROID_MAKE) -C $(KERNEL_OUT_ABS) $(KERNEL_COMPILE_FLAGS) \
		KERNELDIR=$(KERNEL_OUT_ABS) INCSHARED="$(ETHAVB_MSE_KM_INCLUDES)" \
		M=$(ETHAVB_MSE_KM_OUT_ABS) \
		KBUILD_EXTRA_SYMBOLS="$(ETHAVB_MCH_KM_OUT_ABS)/Module.symvers \
		$(ETHAVB_STR_KM_OUT_ABS)/Module.symvers"
	cp $(ETHAVB_MSE_KM_OUT_ABS)/*.ko $(KERNEL_MODULES_OUT)/

# adsp driver
$(ADSP_KM):
	mkdir -p $(ADSP_KM_OUT_ABS)
	cp -pR $(ADSP_KM_SRC)/* $(ADSP_KM_OUT_ABS)/
	$(ANDROID_MAKE) -C $(ADSP_KM_OUT_ABS) $(KERNEL_COMPILE_FLAGS) \
		KERNELDIR=$(KERNEL_OUT_ABS) M=$(ADSP_KM_OUT_ABS)
	cp $(ADSP_KM) $(KERNEL_MODULES_OUT)/

# Realtek 8812au Wi-Fi driver
$(WLAN_KM):
	mkdir -p $(WLAN_KM_OUT_ABS)
	cp -pR $(WLAN_KM_SRC)/* $(WLAN_KM_OUT_ABS)/
	$(ANDROID_MAKE) -C $(WLAN_KM_OUT_ABS) $(KERNEL_COMPILE_FLAGS) \
		KERNELDIR=$(KERNEL_OUT_ABS) \
		WORKDIR=$(WLAN_KM_OUT_ABS) rcar_defconfig
	$(ANDROID_MAKE) -C $(WLAN_KM_OUT_ABS) $(KERNEL_COMPILE_FLAGS) \
		KERNELDIR=$(KERNEL_OUT_ABS) WORKDIR=$(WLAN_KM_OUT_ABS) \
		M=$(WLAN_KM_OUT_ABS) modules
	cp $(WLAN_KM) $(KERNEL_MODULES_OUT)/


KERNEL_EXT_MODULES += \
	$(ROGUE_KM) \
	$(VSPM_KM) \
	$(VSPMIF_KM) \
	$(MMNGR_KM) \
	$(MMNGRBUF_KM) \
	$(UVCS_KM) \
	$(QOS_KM) \
	$(ETHAVB_STR_KM) \
	$(ETHAVB_MCH_KM) \
	$(ETHAVB_MSE_KM) \
	$(WLAN_KM)

ifeq ($(ENABLE_ADSP),true)
	KERNEL_EXT_MODULES += $(ADSP_KM)
endif
