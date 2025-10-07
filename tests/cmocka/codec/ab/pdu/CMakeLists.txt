# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

set(FLATCC_SOURCE_DIR  ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/src)
set(FLATCC_INCLUDE_DIR ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/include)

add_executable(test_codec_ab_pdu
    __test__.c
    test_pdu.c
    test_pdu_can.c
    test_pdu_ip.c
    test_pdu_struct.c
    test_pdu_flexray__engine.c
    test_pdu_flexray__state.c
    test_pdu_flexray__startup.c
    test_pdu_flexray__single_node.c
    test_pdu_flexray__multi_node.c
    test_pdu_flexray__null_frame.c
    flexray/pop/test_flexray_pop.c
    flexray_harness.c
)
target_link_libraries(test_codec_ab_pdu
    PUBLIC
        ab-codec
)
target_include_directories(test_codec_ab_pdu
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${FLATCC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
        .
)
target_compile_definitions(test_codec_ab_pdu
    PUBLIC
        CMOCKA_TESTING
    PRIVATE
        PLATFORM_OS="${CDEF_PLATFORM_OS}"
        PLATFORM_ARCH="${CDEF_PLATFORM_ARCH}"
)
target_link_libraries(test_codec_ab_pdu
    PRIVATE
        cmocka
        dl
        m
)
install(TARGETS test_codec_ab_pdu)
