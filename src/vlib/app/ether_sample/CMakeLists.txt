cmake_minimum_required(VERSION 3.16)
project(ether_sample)

set(RCAR_WRAPPER_APP "")
set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")
set(RCAR_STANDALONE_PATH src/standalone)
set(RCAR_ADDITIONAL_SOURCES 
    src/app_eth_common_sample.c
    src/app_eth_specific_sample.c
)

set(RCAR_INCLUDE_DIRECTORIES
    src
    lib
)

set(RCAR_ADDITIONAL_LIBS
    taurus-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
)

# build and link final executable
include(${CMAKE_VLIB_ROOT}/compiler/build-executable.cmake)
