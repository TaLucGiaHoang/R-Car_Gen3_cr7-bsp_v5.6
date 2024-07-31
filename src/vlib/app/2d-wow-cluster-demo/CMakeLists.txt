cmake_minimum_required(VERSION 3.16)
project(2d-wow-cluster-demo)

# Build 2d-wow-cluster-demo library
add_library(${PROJECT_NAME}
    STATIC
        src/2dwow_main.c
        src/app_cl_carmodel.c
        src/app_cl_gauge.c
        src/app_cluster.c
        src/demo_cluster.c
        ${IMAGE_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        lib
    PRIVATE
        src
        src/misc
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        common
    PRIVATE
        davehd-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        drw2d-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        um_wmlib-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        # Comment out the line below to use pre-built binary of image data
        ${CMAKE_CURRENT_SOURCE_DIR}/src/image/2dwow-image-data-${RCAR_FLOAT_ABI}.a
        m
)