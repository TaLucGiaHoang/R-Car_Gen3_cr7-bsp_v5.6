cmake_minimum_required(VERSION 3.16)
project(drw2d)

# drw source
if(DRW2D_ON_DHD)
    file(GLOB
        DRW2D_PLATFORM_CPU_SOURCES "platform/cpu_reserved/*.c"
        DRW2D_PLATFORM_DAVEHD_SOURCES "platform/davehd/*.c"
        DRW2D_PLATFORM_OS_SOURCES "platform/os/osal/*.c"
        DRW2D_PLATFORM_SRC_SOURCES "src/*.c"
    )

    set(DRW2D_SOURCES
        ${DRW2D_PLATFORM_CPU_SOURCES}
        ${DRW2D_PLATFORM_DAVEHD_SOURCES}
        ${DRW2D_PLATFORM_OS_SOURCES}
        ${DRW2D_PLATFORM_SRC_SOURCES}
    )

    if(DRW2D_SOURCES) # Build DRW2D with source if present
        add_library(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
            STATIC
                ${DRW2D_SOURCES}
        )

        target_include_directories(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
            PUBLIC
                inc
            PRIVATE
                ${CMAKE_VLIB_ROOT}/os/osal/include/rcar-xos/osal
                platform/cpu_reserved
                platform/davehd
                platform/inc
                platform/os/osal
        )

        target_link_libraries(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
            PRIVATE
                davehd-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        )

        # default prefix is "lib"
        set_target_properties(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
            PROPERTIES
                PREFIX ""
        )
    else() # Link existing library if no source code
        add_library(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
            STATIC IMPORTED GLOBAL
        )

        target_include_directories(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
            INTERFACE
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
                ${CMAKE_VLIB_ROOT}/os/osal/include/rcar-xos/osal
        )

        target_link_libraries(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
            INTERFACE
                davehd-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        )

        set_target_properties(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
            PROPERTIES
                PREFIX ""
                IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/${RCAR_TARGET_OS}/${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}.a
        )
    endif()
elseif(DRW2D_ON_CPU)
    file(GLOB
        DRW2D_PLATFORM_CPU_SOURCES "platform/cpu/*.c"
        DRW2D_PLATFORM_DAVEHD_RESERVED_SOURCES "platform/dhd_reserved/r_sys_drw2d_dhd_reserved.c"
        DRW2D_PLATFORM_OS_SOURCES "platform/os/osal/*.c"
        DRW2D_PLATFORM_SRC_SOURCES "src/*.c"
    )

    set(DRW2D_SOURCES
        ${DRW2D_PLATFORM_CPU_SOURCES}
        ${DRW2D_PLATFORM_DAVEHD_RESERVED_SOURCES}
        ${DRW2D_PLATFORM_OS_SOURCES}
        ${DRW2D_PLATFORM_SRC_SOURCES}
    )

    if(DRW2D_SOURCES) # if DRW2D CPU source is present
    include_directories(
        platform/cpu
        platform/dhd_reserved
        platform/inc
        platform/os/osal
    )

    add_library(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        STATIC
            ${DRW2D_SOURCES}
    )

    target_include_directories(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
            ${CMAKE_VLIB_ROOT}/os/osal/include/rcar-xos/osal
    )

    # default prefix is "lib"
    set_target_properties(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        PROPERTIES
            PREFIX ""
    )
    endif()
else()
    message(FATAL_ERROR "ERROR: Improperly configured DRW2D platform")
endif()


