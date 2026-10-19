# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

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
)
target_include_directories(ab-codec-impl
    PUBLIC
        ${FLATCC_INCLUDE_DIR}
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
)
target_link_libraries(ab-codec-impl
    PUBLIC
        ncodec-api
        $<TARGET_OBJECTS:ncodec-api>
        ncodec-stream
        $<TARGET_OBJECTS:ncodec-stream>
        ncodec-flatcc
        $<TARGET_OBJECTS:ncodec-flatcc>
)
