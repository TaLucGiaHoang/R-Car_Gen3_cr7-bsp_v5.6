cmake_minimum_required(VERSION 3.16)
project(rtp_player)

set(RCAR_WRAPPER_APP "")

# # Choose OS dependant for resource_table section in linker file
# if(RCAR_TARGET_GUEST STREQUAL "LINUX_GUEST")
#     set(LINKER_SCRIPT_CONTENT ".resource_table : { *resource_table_linux.c.obj(.resource_table) }")
# else()
#     set(LINKER_SCRIPT_CONTENT ".resource_table (NOLOAD) : { *resource_table_linux.c.obj(.resource_table) }")
# endif()

# # Choose RTOS dependant for specific linker file
# if(RCAR_TARGET_OS STREQUAL "freertos")
#     configure_file(${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld.in ${PROJECT_BINARY_DIR}/${RCAR_TARGET_OS}.ld)
#     set(RCAR_LINKER_SCRIPT "${PROJECT_BINARY_DIR}/${RCAR_TARGET_OS}.ld")
# else()
#     set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")
# endif()

set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")

set(RCAR_STANDALONE_PATH src/standalone)

file(GLOB RTPPLAYER_APP_SOURCES
    src/*.c
    src/avc/*.c
    src/rtp/*.c
)

set(RCAR_ADDITIONAL_SOURCES
    ${RTPPLAYER_APP_SOURCES}
)

set(RCAR_INCLUDE_DIRECTORIES
    include
    lib
    src
    src/avc
    src/rtp
)

set(RCAR_ADDITIONAL_LIBS
    um_wmlib-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    taurus-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    rivp-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    lwip-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
)

# build and link final executable
include(${CMAKE_VLIB_ROOT}/compiler/build-executable.cmake)
