cmake_minimum_required(VERSION 3.16)
project(logo)

set(RCAR_WRAPPER_APP "")
set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")
set(RCAR_STANDALONE_PATH src/standalone)

# Build logo library
add_library(${PROJECT_NAME}-lib
    src/logo_main.c
    src/image_hdmi/logo_image_hdmi.c
    src/image_lvds/logo_image_lvds.c
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