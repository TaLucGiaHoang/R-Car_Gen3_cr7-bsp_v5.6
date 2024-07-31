cmake_minimum_required(VERSION 3.16)
project(drv_wmlib)

add_library(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    STATIC
        src/r_wmdrv_api.c
        src/rcarx3-ros/r_wmdrv_sys_irq.c
        src/rcarx3-ros/r_wmdrv_sys.c
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
    PRIVATE
        cio-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        drivers-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        hdmi-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}-${RCAR_FPU}
)

# default prefix is "lib"
set_target_properties(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS} PROPERTIES PREFIX "")
