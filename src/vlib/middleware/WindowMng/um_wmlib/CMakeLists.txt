cmake_minimum_required(VERSION 3.16)
project(um_wmlib)

add_library(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    STATIC
        src/r_wm_api.c
        src/r_wm_msg_com.c      
)

target_include_directories(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    PUBLIC
        lib
        ../bridge/lib
        ../bridge/lib/config/rcarx3
    PRIVATE
        ${CMAKE_VLIB_ROOT}/os/osal/include
)

target_link_libraries(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    um_ciolib-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
)

# default prefix is "lib"
set_target_properties(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS} PROPERTIES PREFIX "")
