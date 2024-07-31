cmake_minimum_required(VERSION 3.16)
project(imr_sample)

set(RCAR_WRAPPER_APP "")
set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")
set(RCAR_STANDALONE_PATH src)
set(RCAR_ADDITIONAL_SOURCES 
    common/application/src/p2c_1280_15_dl.c
    common/application/src/p2c_1280_15_dst_yuvsep.c
    common/application/src/p2c_1280_15_yuvsep.c
    imrlxsample/application/src/app_main.c
    common/application/src/settings_h3.c
)

set(RCAR_INCLUDE_DIRECTORIES
    src
    lib
    ${CMAKE_VLIB_ROOT}/os/osal/include
    common/application/include
)

add_compile_definitions(
    MAX_MODULE=4
    IMRSMP_YUVSEP_YONLY
)

# build and link final executable
set(RCAR_ADDITIONAL_LIBS
    taurus-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
)

include(${CMAKE_VLIB_ROOT}/compiler/build-executable.cmake)