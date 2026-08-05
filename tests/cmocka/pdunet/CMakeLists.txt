# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

set(FLATCC_SOURCE_DIR  ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/src)
set(FLATCC_INCLUDE_DIR ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/include)

add_executable(test_pdunet
    __test__.c
    test_pdunet_network.c
    test_pdunet_sigmap.c
    test_pdunet_ncodec.c
    test_pdunet_schedule.c
)
target_link_libraries(test_pdunet
    PUBLIC
        pdunet
    PRIVATE
        ab-codec
        yaml
        lua
        cmocka
        dl
        m
)
target_include_directories(test_pdunet
    PRIVATE
        ${FLATCC_INCLUDE_DIR}
        ${DSE_PDUNET_INCLUDE_DIR}
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
        ${LUA_SOURCE_DIR}
        ${YAML_SOURCE_DIR}/include
)
target_compile_definitions(test_pdunet
    PUBLIC
        CMOCKA_TESTING
    PRIVATE
        PLATFORM_OS="${CDEF_PLATFORM_OS}"
        PLATFORM_ARCH="${CDEF_PLATFORM_ARCH}"
)
install(TARGETS test_pdunet)
install(
    FILES
        pdunet.yaml
        pdunet_container.yaml
        pdunet_lua.yaml
        pdunet_secured.yaml
        pdunet_frnet.yaml
        pdunet_frcontainer.yaml
    DESTINATION
        resources/model
)



# Targets
# =======
add_subdirectory(../../../dse/pdunet pdunet)
