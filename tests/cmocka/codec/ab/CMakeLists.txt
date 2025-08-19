# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

set(FLATCC_SOURCE_DIR  ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/src)
set(FLATCC_INCLUDE_DIR ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/include)

add_executable(test_codec_ab
    __test__.c
    test_codec.c
    ${DSE_NCODEC_SOURCE_DIR}/stream/buffer.c
    ${DSE_NCODEC_SOURCE_DIR}/codec/ab/codec.c
    ${DSE_NCODEC_SOURCE_DIR}/codec/ab/frame_fbs.c
    ${DSE_NCODEC_SOURCE_DIR}/codec/ab/pdu_fbs.c
    ${DSE_NCODEC_SOURCE_DIR}/codec/ab/flexray/engine.c
    ${DSE_NCODEC_SOURCE_DIR}/codec/ab/flexray/fbs.c
    ${DSE_NCODEC_SOURCE_DIR}/codec/ab/flexray/state.c
    ${DSE_NCODEC_SOURCE_DIR}/codec/ab/flexray/flexray.c
    ${DSE_NCODEC_SOURCE_DIR}/codec.c
    ${FLATCC_SOURCE_DIR}/builder.c
    ${FLATCC_SOURCE_DIR}/emitter.c
    ${FLATCC_SOURCE_DIR}/refmap.c
)
target_include_directories(test_codec_ab
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${FLATCC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
)
target_compile_definitions(test_codec_ab
    PUBLIC
        CMOCKA_TESTING
    PRIVATE
        PLATFORM_OS="${CDEF_PLATFORM_OS}"
        PLATFORM_ARCH="${CDEF_PLATFORM_ARCH}"
)
target_link_libraries(test_codec_ab
    PRIVATE
        cmocka
        dl
        m
)
install(TARGETS test_codec_ab)



# Targets
# =======

add_subdirectory(frame)
add_subdirectory(pdu)
