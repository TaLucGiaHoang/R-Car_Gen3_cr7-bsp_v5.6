cmake_minimum_required(VERSION 3.16)
project(cluster_wrapper)

set(RCAR_WRAPPER_APP "2d-wow-cluster-demo")
set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")
set(RCAR_STANDALONE_PATH src)
set(RCAR_ADDITIONAL_SOURCES 
    src/cluster_cfg.c
)

set(RCAR_ADDITIONAL_LIBS
    2d-wow-cluster-demo
    taurus-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
)

# build and link final executable
include(${CMAKE_VLIB_ROOT}/compiler/build-executable.cmake)
