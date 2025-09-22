# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

set(FLATCC_SOURCE_DIR  ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/src)
set(FLATCC_INCLUDE_DIR ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/include)

add_executable(test_codec_ab_frame
    __test__.c
    test_can_fbs.c
)
target_link_libraries(test_codec_ab_frame
    PUBLIC
        ab-codec
)
target_include_directories(test_codec_ab_frame
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${FLATCC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
)
target_compile_definitions(test_codec_ab_frame
    PUBLIC
        CMOCKA_TESTING
    PRIVATE
        PLATFORM_OS="${CDEF_PLATFORM_OS}"
        PLATFORM_ARCH="${CDEF_PLATFORM_ARCH}"
)
target_link_libraries(test_codec_ab_frame
    PRIVATE
        cmocka
        dl
        m
)
install(TARGETS test_codec_ab_frame)
