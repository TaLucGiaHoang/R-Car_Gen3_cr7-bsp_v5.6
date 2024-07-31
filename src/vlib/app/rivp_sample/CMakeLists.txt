cmake_minimum_required(VERSION 3.16)
project(rivp_sample)

# Only one option may be set
#  Note: Delete CMake cache when changing options
option(JPEG_DECODER "Jpeg Decoder" ON)
option(AVC_DECODER "AVC Decoder" OFF)

if(JPEG_DECODER)
    if(AVC_DECODER)
        message(FATAL_ERROR "${project}: Invalid configuration")
    else()
        add_compile_definitions(
            RIVP_DEP_LDD_ENABLE_JPEG
        )
        file(GLOB
            DECODER_SOURCES "src/jpg/*.c"
        )
    endif()
elseif(AVC_DECODER)
    file(GLOB
      DECODER_SOURCES "src/avc/*.c"
    )
else()
    message(FATAL_ERROR "${project}: Invalid configuration")
endif()

file(GLOB
    SOURCES "src/*.c"
)

set(RCAR_WRAPPER_APP "")
set(RCAR_LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/target/${RCAR_TARGET_OS}_salvatorxs/${RCAR_TARGET_OS}.ld")
set(RCAR_STANDALONE_PATH src/standalone)
set(RCAR_ADDITIONAL_SOURCES
    ${SOURCES}
    ${DECODER_SOURCES}
)
set(RCAR_ADDITIONAL_LIBS 
    taurus-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    rivp-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
)

set(RCAR_INCLUDE_DIRECTORIES
    include
    lib
    src
    src/jpg
    src/avc
)

# build and link final executable
include(${CMAKE_VLIB_ROOT}/compiler/build-executable.cmake)