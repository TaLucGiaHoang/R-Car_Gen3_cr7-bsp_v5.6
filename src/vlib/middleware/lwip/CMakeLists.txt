cmake_minimum_required(VERSION 3.16)
project(lwip)

include_directories(
    ${CMAKE_VLIB_ROOT}/os/osal/include
    # ${CMAKE_VLIB_ROOT}/drivers/print/lib
)

file(GLOB
    API_SOURCES "lwip-2.1.3/src/api/*.c"
    CORE_SOURCES "lwip-2.1.3/src/core/*.c"
    IPV4_SOURCES "lwip-2.1.3/src/core/ipv4/*.c"
    IPV6_SOURCES "lwip-2.1.3/src/core/ipv6/*.c"
    PORT_SOURCES "port/*.c"
    PORT_NETIF_SOURCES "port/netif/*.c"
)

set(LWIP_SOURCES
    ${API_SOURCES}
    ${CORE_SOURCES}
    ${IPV4_SOURCES}
    ${IPV6_SOURCES}
    ${PORT_SOURCES}
    ${PORT_NETIF_SOURCES}
)

add_library(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    STATIC
        ${LWIP_SOURCES}
)

target_include_directories(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    PUBLIC
        lib
        lwip-2.1.3/src/include/
        lwip-2.1.3/src/include/lwip
        lwip-2.1.3/src/include/netif
        lwip-2.1.3/src/include/compat/posix
        lwip-2.1.3/src/include/compat/stdc
        port/include
        port/include/arch
        port/include/netif
)

target_link_libraries(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
    PRIVATE
        drivers-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS}
        # common
)

# default prefix is "lib"
set_target_properties(${PROJECT_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_COMPILER}-${RCAR_TARGET_OS} PROPERTIES PREFIX "")