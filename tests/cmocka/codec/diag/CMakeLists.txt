# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

set(FLATCC_INCLUDE_DIR ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/include)

add_executable(test_codec_diag
    __test__.c
    test_codec.c
    test_uds.c
    test_tp.c
    test_srpc.c
    test_xil.c
    test_uds_variants.c
    test_xil_variants.c
)
target_link_libraries(test_codec_diag
    PUBLIC
        ab-codec
)
target_include_directories(test_codec_diag
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${FLATCC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
        .
)
target_compile_definitions(test_codec_diag
    PUBLIC
        CMOCKA_TESTING
)
target_link_libraries(test_codec_diag
    PRIVATE
        cmocka
        dl
        m
)
install(TARGETS test_codec_diag)
