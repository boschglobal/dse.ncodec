# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(FLATCC_SOURCE_DIR  ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/src)
set(FLATCC_INCLUDE_DIR ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/include)


# Targets
# =======

# Target - Automotive Bus Codec
# -----------------------------
add_library(ab-codec OBJECT
        codec.c
        frame_fbs.c
        pdu_fbs.c
        ${DSE_NCODEC_SOURCE_DIR}/codec.c
        ${FLATCC_SOURCE_DIR}/builder.c
        ${FLATCC_SOURCE_DIR}/emitter.c
        ${FLATCC_SOURCE_DIR}/refmap.c
)
target_include_directories(ab-codec
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${FLATCC_INCLUDE_DIR}
)
