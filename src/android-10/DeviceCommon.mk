#
# Copyright (C) 2018 GlobalLogic
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

$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, device/renesas/common/build/car.mk)

# Adjust the Dalvik heap to be appropriate for a tablet.
$(call inherit-product, frameworks/native/build/tablet-10in-xhdpi-2048-dalvik-heap.mk)

# Build and run only ART
PRODUCT_RUNTIMES := runtime_libart_default
PRODUCT_CHARACTERISTICS := tablet,nosdcard
PRODUCT_SHIPPING_API_LEVEL := 29
PRODUCT_OTA_ENFORCE_VINTF_KERNEL_REQUIREMENTS := false

# Add preffered configurations
PRODUCT_AAPT_CONFIG := normal mdpi large xlarge hdpi xhdpi
PRODUCT_AAPT_PREF_CONFIG := mdpi

# Overlays for device
DEVICE_PACKAGE_OVERLAYS += \
    device/renesas/common/overlay

PRODUCT_PACKAGE_OVERLAYS += \
    device/renesas/$(TARGET_PRODUCT)/overlay

PRODUCT_ENFORCE_RRO_TARGETS := framework-res

# The default locale should be determined from VPD, not from build.prop.
PRODUCT_SYSTEM_PROPERTY_BLACKLIST := \
    ro.product.locale

# OEM Unlock reporting
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    ro.oem_unlock_supported=1

# Use SdcardFS
PRODUCT_PRODUCT_PROPERTIES += \
    ro.sys.sdcardfs=1

# Enable runtime enforcement
PRODUCT_PROPERTY_OVERRIDES += \
    ro.control_privapp_permissions=enforce

# Disable CameraServiceProxy for all boards by default
PRODUCT_PROPERTY_OVERRIDES += \
    config.disable_cameraservice=true

# ----------------------------------------------------------------------
PRODUCT_COPY_FILES += \
    device/renesas/common/permissions/android.software.home_screen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.home_screen.xml \
    frameworks/native/data/etc/android.software.connectionservice.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.connectionservice.xml \
    frameworks/native/data/etc/android.software.voice_recognizers.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.voice_recognizers.xml \
    frameworks/native/data/etc/android.software.backup.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.backup.xml \
    frameworks/native/data/etc/android.software.webview.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.webview.xml \
    frameworks/native/data/etc/android.software.activities_on_secondary_displays.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.activities_on_secondary_displays.xml \
    frameworks/native/data/etc/android.software.picture_in_picture.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.picture_in_picture.xml \
    frameworks/native/data/etc/android.software.cts.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.cts.xml \
    frameworks/native/data/etc/android.software.secure_lock_screen.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.secure_lock_screen.xml

PRODUCT_COPY_FILES += \
    device/renesas/common/permissions/android.software.cant_save_state.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.cant_save_state.xml \
    device/renesas/common/permissions/android.hardware.microphone.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.microphone.xml \
    device/renesas/common/permissions/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.audio.output.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.output.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.ethernet.xml \
    frameworks/native/data/etc/android.hardware.screen.landscape.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.screen.landscape.xml \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.hardware.opengles.aep.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.opengles.aep.xml \
    frameworks/native/data/etc/android.software.ipsec_tunnels.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.ipsec_tunnels.xml \
    frameworks/native/data/etc/android.software.verified_boot.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.verified_boot.xml

# RCarRadio dependent package
PRODUCT_COPY_FILES += \
    frameworks/base/data/etc/car/com.android.car.radio.xml:$(TARGET_COPY_OUT_PRODUCT)/etc/permissions/com.android.car.radio.xml

# Used to embed a map in an activity view
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.activities_on_secondary_displays.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.activities_on_secondary_displays.xml

# Enable telephony carrier restriction mechanism
PRODUCT_COPY_FILES += \
    device/renesas/common/permissions/android.hardware.telephony.carrierlock.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.carrierlock.xml

# Default app permissions
PRODUCT_COPY_FILES += \
    device/renesas/common/permissions/default-permissions.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default-permissions/default-permissions.xml

# Init RC files
PRODUCT_COPY_FILES += \
    device/renesas/common/init/init.common.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.common.rc \
    device/renesas/common/init/init.common.usb.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.common.usb.rc \
    packages/services/Car/car_product/init/init.bootstat.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/hw/init.bootstat.rc

# Custom keylayout for Renesas
PRODUCT_COPY_FILES += \
    device/renesas/common/gpio_keylayout.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/gpio_keys.kl

# seccomp policy
PRODUCT_PACKAGES += \
    mediacodec.policy \
    crash_dump.policy

PRODUCT_COPY_FILES += \
    device/renesas/common/seccomp/mediacodec-seccomp.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediacodec.policy \
    device/renesas/common/seccomp/mediaextractor-seccomp.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediaextractor.policy \
    device/renesas/common/seccomp/mediaswcodec-seccomp.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediaswcodec.policy

ifneq ($(ANDROID_MMC_ONE_SLOT),true)
PRODUCT_COPY_FILES += \
    device/renesas/common/fstab.fbe:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.$(TARGET_PRODUCT) \
    device/renesas/common/fstab.fbe:$(TARGET_COPY_OUT_RECOVERY)/root/first_stage_ramdisk/fstab.$(TARGET_PRODUCT)
else
PRODUCT_COPY_FILES += \
    device/renesas/common/fstab.fbe.one_slot:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.$(TARGET_PRODUCT) \
    device/renesas/common/fstab.fbe.one_slot:$(TARGET_COPY_OUT_RECOVERY)/root/first_stage_ramdisk/fstab.$(TARGET_PRODUCT)
endif

# For dynamic partitions
PRODUCT_USE_DYNAMIC_PARTITIONS := true
PRODUCT_BUILD_SUPER_PARTITION := true

PRODUCT_PACKAGES += \
    fastbootd \
    android.hardware.boot@1.0-impl-renesas

PRODUCT_COPY_FILES += \
    device/renesas/$(TARGET_PRODUCT)/media/media_profiles.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_V1_0.xml

# C2 HAL
PRODUCT_PACKAGES += \
    android.hardware.media.c2@1.0-service.renesas

# Touchcreen configuration
PRODUCT_COPY_FILES += \
    device/renesas/common/input-port-associations-skeleton.xml:$(TARGET_COPY_OUT_VENDOR)/etc/input-port-associations-skeleton.xml

# Graphics
PRODUCT_PACKAGES += \
    android.hardware.graphics.common@1.0-impl \
    android.hardware.graphics.mapper@3.0-impl \
    android.hardware.graphics.allocator@3.0-impl \
    android.hardware.graphics.allocator@3.0-service \
    android.hardware.graphics.composer@2.3-service.renesas \
    powervr_prebuilts

# Render Script
PRODUCT_PACKAGES += \
    android.hardware.renderscript@1.0-impl \
    librs_jni \
    libLLVM

OVERRIDE_RS_DRIVER := libPVRRS.so

# Audio common defaults
$(call inherit-product, vendor/renesas/hal/audio/car_audio.mk)

PRODUCT_PACKAGES += \
    audio.a2dp.default \
    android.hardware.audio.effect@5.0-service.renesas

ifeq ($(ENABLE_ADSP),true)
PRODUCT_PACKAGES += xf-rcar.fw
endif

PRODUCT_COPY_FILES += \
    frameworks/av/media/libeffects/data/audio_effects.conf:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects.conf

# OPTee
PRODUCT_PACKAGES += \
    tee-supp \
    tee-supp_recovery

PRODUCT_PACKAGES += \
    hyper_ca \
    hyper_ca_legacy \

PRODUCT_HOST_PACKAGES += \
    pack_ipl_emmc

# ----------------------------------------------------------------------
PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=196610 \
    ro.radio.noril=true \
    ro.carrier=unknown

# Set encryption algorithm for content of an adoptable storage
PRODUCT_PROPERTY_OVERRIDES += \
    ro.crypto.volume.filenames_mode=aes-256-cts

ifeq ($(TARGET_USES_HIGHEST_DPI),true)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=240
else
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=160
endif

# Set default USB interface
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp

PRODUCT_TAGS += dalvik.gc.type-precise

PRODUCT_PACKAGES += com.android.future.usb.accessory

# QoS
PRODUCT_PACKAGES += \
    libqos

# Health HAL
PRODUCT_PACKAGES += \
   android.hardware.health@2.0-service.renesas

# Keymaster HAL
PRODUCT_PACKAGES += \
    android.hardware.keymaster@3.0-service.renesas

# Gatekeeper HAL
PRODUCT_PACKAGES += \
    android.hardware.gatekeeper@1.0-service.renesas

# OemLock HAL
PRODUCT_PACKAGES += \
    android.hardware.oemlock@1.0-service.renesas

# Vehicle HAL
PRODUCT_PACKAGES += \
    android.hardware.automotive.vehicle@2.0-service.renesas

# EVS resources
PRODUCT_PACKAGES += \
    android.hardware.automotive.evs@1.0-service.renesas

PRODUCT_COPY_FILES += \
    vendor/renesas/hal/evs/config.$(TARGET_PRODUCT).json:$(TARGET_COPY_OUT_VENDOR)/etc/automotive/evs/config.json

PRODUCT_PACKAGES += \
    evs_app.gl

PRODUCT_PROPERTY_OVERRIDES += \
    ro.boot.evs.app=google

# GNSS
PRODUCT_PACKAGES += \
    android.hardware.gnss@2.0-service.renesas

PRODUCT_COPY_FILES += \
    vendor/renesas/hal/gnss/1.0/fake_route.txt:$(TARGET_COPY_OUT_VENDOR)/etc/fake_route.txt

# USB HAL
PRODUCT_PACKAGES += \
    android.hardware.usb@1.0-service.renesas \
    android.hardware.usb.gadget@1.0-service.renesas

# USB3 firmwares
PRODUCT_PACKAGES += \
    r8a779x_usb3_v2.dlmem \
    r8a779x_usb3_v3.dlmem

# Wi-Fi
PRODUCT_PACKAGES += \
    android.hardware.wifi.supplicant@1.0-service \
    android.hardware.wifi.supplicant@1.1-service \
    android.hardware.wifi.supplicant@1.2-service \
    hostapd \
    wlutil \
    wificond \
    wpa_supplicant \
    wpa_supplicant.conf

# Bluetooth
PRODUCT_COPY_FILES += \
    device/renesas/common/init/scripts/android.hardware.bluetooth@1.0-service.renesas.sh:$(TARGET_COPY_OUT_VENDOR)/etc/init/scripts/android.hardware.bluetooth@1.0-service.renesas.sh

# Generic memtrack module
PRODUCT_PACKAGES += \
    android.hardware.memtrack@1.0-impl \
    android.hardware.memtrack@1.0-service

# Vehicle HAL
RODUCT_PACKAGES += \
    android.hardware.automotive.vehicle@2.0-service.$(TARGET_PRODUCT)

# DRM HAL
PRODUCT_PACKAGES += \
    android.hardware.drm@1.0-impl \
    android.hardware.drm@1.0-service \
    android.hardware.drm@1.2-service.clearkey \

# Generic HALs
PRODUCT_PACKAGES += \
    android.hardware.drm@1.0-service.renesas \
    android.hardware.light@2.0-service.renesas \
    android.hardware.contexthub@1.0-service.renesas \
    android.hardware.thermal@1.1-service.renesas \
    android.hardware.dumpstate@1.0-service.renesas

# Boot control HAL (libavb)
PRODUCT_PACKAGES += \
    android.hardware.boot@1.0-service.renesas

#Fastboot HAL
PRODUCT_PACKAGES += \
    android.hardware.fastboot@1.0-impl-renesas

# A/B System Updates
ifneq ($(ANDROID_MMC_ONE_SLOT),true)
    AB_OTA_UPDATER := true
endif
AB_OTA_PARTITIONS := \
    boot \
    system \
    vendor \
    vbmeta \
    dtbo \
    product \
    odm

PRODUCT_PACKAGES += \
    update_verifier \
    update_engine

# A/B recovery boot control HAL (libavb)
PRODUCT_STATIC_BOOT_CONTROL_HAL := \
    libavb_user \
    libavb \
    libfs_mgr

# A/B OTA dexopt package
PRODUCT_PACKAGES += \
    otapreopt_script

# A/B OTA dexopt update_engine hookup
AB_OTA_POSTINSTALL_CONFIG += \
    RUN_POSTINSTALL_system=true \
    POSTINSTALL_PATH_system=system/bin/otapreopt_script \
    FILESYSTEM_TYPE_system=ext4 \
    POSTINSTALL_OPTIONAL_system=true

# A/B debug utilities
PRODUCT_PACKAGES_DEBUG += \
    bootctrl \
    update_engine_client

# I2C utility
PRODUCT_PACKAGES += \
    i2cset

# IRQ balancer utility
PRODUCT_PACKAGES += \
    irqbalance

# Enabling number blocking
PRODUCT_PACKAGES += \
    BlockedNumberProvider

# ----------------------------------------------------------------------
ifneq (,$(filter userdebug eng, $(TARGET_BUILD_VARIANT)))

# Set default log size on userdebug/eng builds to 1M
PRODUCT_PROPERTY_OVERRIDES += ro.logd.size=1M

# For VTS profiling.
PRODUCT_PACKAGES += \
    libvts_profiling \
    libvts_multidevice_proto

# For ALSA testing
PRODUCT_PACKAGES += \
    tinyplay \
    tinycap \
    tinymix \
    tinypcminfo

# For QoS testing
PRODUCT_PACKAGES += \
    qos_tp

endif # TARGET_BUILD_VARIANT

# For ion memory control
ifeq ($(ENABLE_ION_LMK),true)
PRODUCT_PACKAGES += \
    ion_lmk

# Enable ion lmk service by default
PRODUCT_PRODUCT_PROPERTIES += \
    ro.boot.ion_lmk_start=1
endif

# USB Port Config
PRODUCT_COPY_FILES += \
   device/renesas/$(TARGET_PRODUCT)/usb_port_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/usb_port_configuration.xml

# ----------------------------------------------------------------------
# Split selinux policy
PRODUCT_FULL_TREBLE_OVERRIDE := true

# All VNDK libraries (HAL interfaces, VNDK, VNDK-SP, LL-NDK)
PRODUCT_PACKAGES += vndk_package

# CMS(Color Management System)
ifeq ($(ENABLE_CMS),true)
PRODUCT_PACKAGES += \
    vendor.renesas.graphics.cms@1.0-service.renesas \
    CmsTest
endif

# ----------------------------------------------------------------------
$(call inherit-product, frameworks/base/data/sounds/AudioPackage5.mk)
$(call inherit-product-if-exists, vendor/renesas/firmware/remoteproc/remoteproc.mk)

ifneq (,$(wildcard vendor/google/products/gms.mk))
    $(call inherit-product, vendor/google/products/gms.mk)

# Enable Network location provider only in build with GMS
    PRODUCT_COPY_FILES += \
        device/renesas/common/permissions/android.hardware.location.network.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.network.xml
endif

PRODUCT_PACKAGES += \
    netutils-wrapper-1.0

# 32-bit library for 32-bit VtsKernelNetdTest
PRODUCT_PACKAGES += libssl_32

# ----------------------------------------------------------------------
# Please keep these records at end of file DeviceCommon.mk

# Define cross-compiler for BSP-Projects: Linux Kernel, OP-TEE, TAs, U-BOOT, IPLs
BSP_GCC_CROSS_COMPILE       := $(abspath ./prebuilts/gcc/linux-x86/aarch64/aarch64-linux-gnu/bin/aarch64-linux-gnu-)
BSP_GCC_HOST_TOOLCHAIN      := $(abspath ./prebuilts/gcc/linux-x86/host/x86_64-linux-glibc2.17-4.8/bin/x86_64-linux-)
# Clang prebuilts March 2019
ANDROID_CLANG_TOOLCHAIN     := $(abspath ./prebuilts/clang/host/linux-x86/clang-r353983c/bin/clang)
# Prebuilt Make program
ANDROID_MAKE                := $(abspath ./prebuilts/build-tools/linux-x86/bin/make)
