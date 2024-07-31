cmake_minimum_required(VERSION 3.16)
project(davehd)

# dhd source
file(GLOB
    DHD_COMMON_SOURCES "common/src/*.c"
    DHD_DEBUG_SOURCES "debug/*.c"
    DHD_DRIVER_SOURCES "driver/*.c"
    DHD_KERNEL_PLATFORM_SOURCES "kernel/platform/*.c"
    DHD_KERNEL_SOURCES "kernel/src/*.c"
    DHD_SERVER_SOURCES "server/src/*.c"
    DHD_USER_PLATFORM_SOURCES "user/platform/*.c"
    DHD_USER_PLATFORM_SRC_SOURCES "user/platform/src/*.c"
    DHD_USER_SOURCES "user/src/*.c"
)

set(DHD_SOURCES
    ${DHD_COMMON_SOURCES}
    ${DHD_DEBUG_SOURCES}
    ${DHD_DRIVER_SOURCES}
    ${DHD_KERNEL_PLATFORM_SOURCES}
    ${DHD_KERNEL_SOURCES}
    ${DHD_SERVER_SOURCES}
    ${DHD_USER_PLATFORM_SOURCES}
    ${DHD_USER_PLATFORM_SRC_SOURCES}
    ${DHD_USER_SOURCES}
)

if(DHD_SOURCES) # Build DHD with source if present
    add_library(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        STATIC
            ${DHD_SOURCES}
    )

    target_include_directories(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        PUBLIC
            common/inc
            inc
            kernel/inc
        PRIVATE
            ${CMAKE_VLIB_ROOT}/os/osal/include/rcar-xos/osal
            common/src
            kernel/src
            server/src
            user/src
            user/platform
            kernel/platform
            kernel/platform/cr7_osal
            common/inc/platform
    )

    target_link_libraries(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        PRIVATE
            drivers-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    )

    # default prefix is "lib"
    set_target_properties(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS} PROPERTIES PREFIX "")
else() # Link existing library if no source code
    add_library(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        STATIC IMPORTED GLOBAL
    )

    target_include_directories(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/inc>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/kernel/inc>
            ${CMAKE_VLIB_ROOT}/os/osal/include/rcar-xos/osal
    )

    set_target_properties(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        PROPERTIES
            PREFIX ""
            IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${RCAR_TARGET_OS}/${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}.a
    )
endif()