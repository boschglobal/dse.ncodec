# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

set(FLATCC_SOURCE_DIR  ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/src)
set(FLATCC_INCLUDE_DIR ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/include)
# set(CMAKE_VERBOSE_MAKEFILE ON)


# Targets
# =======

# Target - Automotive Bus Codec
# -----------------------------
add_library(ab-codec-impl OBJECT
    codec.c
    frame_fbs.c
    pdu_fbs.c
    flexray/engine.c
    flexray/fbs.c
    flexray/state.c
    flexray/flexray.c
    flexray_pop/flexray_pop.c
    ${DSE_NCODEC_SOURCE_DIR}/codec.c
    ${DSE_NCODEC_SOURCE_DIR}/stream/ascii85.c
    ${DSE_NCODEC_SOURCE_DIR}/stream/buffer.c
    ${FLATCC_SOURCE_DIR}/builder.c
    ${FLATCC_SOURCE_DIR}/emitter.c
    ${FLATCC_SOURCE_DIR}/refmap.c
)
target_include_directories(ab-codec-impl
    PUBLIC
        ${FLATCC_INCLUDE_DIR}
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
)
