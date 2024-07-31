cmake_minimum_required(VERSION 3.16)
project(fastboot_mono)

set(RCAR_WRAPPER_APP "")
set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")
set(RCAR_STANDALONE_PATH src)

cmake_dependent_option(FASTBOOT_CFG_APP_AUDIO_ENABLE "Enables audio sub-application" OFF "R_USE_AUDIO" OFF)
cmake_dependent_option(FASTBOOT_CFG_APP_CAMERA_ENABLE "Enables camera sub-application" OFF "R_USE_VIN" OFF)
# option(FASTBOOT_CFG_APP_CAN_ENABLE "Enables CAN sub-application" OFF)
option(FASTBOOT_CFG_APP_CLUSTER_ENABLE "Enables 2D WoW Cluster sub-application" ON)
# option(FASTBOOT_CFG_APP_GUIDELINE_ENABLE "Enables rearview camera guideline sub-application" OFF)
option(FASTBOOT_CFG_APP_LOGO_ENABLE "Enables logo sub-application" OFF)
option(FASTBOOT_CFG_APP_SPLASH_SCREEN_ENABLE "Enables animated logo sub-application" ON)

# Conditional compile definitions
#  To enable a feature, use 'cmake -D<feature> <build-folder>'
#  Example for camera sub app: cmake -DR_USE_VIN=ON -DFASTBOOT_CFG_APP_CAMERA_ENABLE build-freertos
add_compile_definitions(
    $<$<BOOL:${FASTBOOT_CFG_APP_AUDIO_ENABLE}>:FASTBOOT_CFG_APP_AUDIO_ENABLE>
    $<$<BOOL:${FASTBOOT_CFG_APP_CAMERA_ENABLE}>:FASTBOOT_CFG_APP_CAMERA_ENABLE>
    $<$<BOOL:${FASTBOOT_CFG_APP_CAN_ENABLE}>:FASTBOOT_CFG_APP_CAN_ENABLE>
    $<$<BOOL:${FASTBOOT_CFG_APP_CLUSTER_ENABLE}>:FASTBOOT_CFG_APP_CLUSTER_ENABLE>
    $<$<BOOL:${FASTBOOT_CFG_APP_GUIDELINE_ENABLE}>:FASTBOOT_CFG_APP_GUIDELINE_ENABLE>
    $<$<BOOL:${FASTBOOT_CFG_APP_LOGO_ENABLE}>:FASTBOOT_CFG_APP_LOGO_ENABLE>
    $<$<BOOL:${FASTBOOT_CFG_APP_SPLASH_SCREEN_ENABLE}>:FASTBOOT_CFG_APP_SPLASH_SCREEN_ENABLE>
)

# Build application library
add_library(${PROJECT_NAME}-lib
    src/audio/audio_cfg.c
    src/camera/camera_cfg.c
    src/camera/rvc_cfg.c
    src/cluster/cluster_cfg.c
    src/logo/logo_cfg.c
    src/splash_screen/splash_screen_cfg.c
    src/service/service_cfg.c
)

target_include_directories(${PROJECT_NAME}-lib
    PUBLIC
        lib
    PRIVATE
        src
)

target_link_libraries(${PROJECT_NAME}-lib
    PUBLIC
        um_wmlib-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        common
        taurus-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    PRIVATE
        cio-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        # Conditional library inclusions
        $<$<BOOL:${FASTBOOT_CFG_APP_AUDIO_ENABLE}>:audio-lib>
        $<$<BOOL:${FASTBOOT_CFG_APP_CLUSTER_ENABLE}>:2d-wow-cluster-demo>
        $<$<BOOL:${FASTBOOT_CFG_APP_LOGO_ENABLE}>:logo-lib>
        $<$<BOOL:${FASTBOOT_CFG_APP_CAMERA_ENABLE}>:rearview_camera-lib>
        $<$<BOOL:${FASTBOOT_CFG_APP_SPLASH_SCREEN_ENABLE}>:dcu_sample-lib>
)

# build and link final executable
set(RCAR_ADDITIONAL_LIBS
    ${PROJECT_NAME}-lib
)

include(${CMAKE_VLIB_ROOT}/compiler/build-executable.cmake)
