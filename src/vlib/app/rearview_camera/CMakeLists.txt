cmake_minimum_required(VERSION 3.16)
project(rearview_camera)

set(RCAR_WRAPPER_APP "")
set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")
set(RCAR_STANDALONE_PATH src/standalone)
set(RCAR_ADDITIONAL_SOURCES 
    src/rearview_cfg.c
)

set(RCAR_INCLUDE_DIRECTORIES
    src
    lib
)

# Build camera library
add_library(${PROJECT_NAME}-lib
    src/rearview_main.c
)

target_include_directories(${PROJECT_NAME}-lib
    PUBLIC
        lib
    PRIVATE
        src
)

target_link_libraries(${PROJECT_NAME}-lib
    PRIVATE
        um_ciolib-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        um_wmlib-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        common
)

set_target_properties(${PROJECT_NAME}-lib PROPERTIES PREFIX "")

# build and link final executable
set(RCAR_ADDITIONAL_LIBS
    ${PROJECT_NAME}-lib
    taurus-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
)

include(${CMAKE_VLIB_ROOT}/compiler/build-executable.cmake)
