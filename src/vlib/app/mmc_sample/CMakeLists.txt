cmake_minimum_required(VERSION 3.16)
project(mmc_sample)

set(RCAR_WRAPPER_APP "")
set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")
set(RCAR_STANDALONE_PATH src/standalone)
set(RCAR_ADDITIONAL_SOURCES 
    src/mmc_main.c
)

set(RCAR_INCLUDE_DIRECTORIES
    src
    lib
    ${CMAKE_VLIB_ROOT}/os/osal_wrapper/src/include
)

# build and link final executable
set(RCAR_ADDITIONAL_LIBS
    taurus-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
)

include(${CMAKE_VLIB_ROOT}/compiler/build-executable.cmake)