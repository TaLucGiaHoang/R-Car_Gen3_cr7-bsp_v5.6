cmake_minimum_required(VERSION 3.16)
project(hello_rgl)

add_library(${PROJECT_NAME}
    STATIC
        src/draw.c
        src/gpu.c
        src/hello_rgl_main.c
        src/vout.c
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        lib
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        common
        davehd-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        drw2d-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        um_wmlib-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        m # math library is needed
)
