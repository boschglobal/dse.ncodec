# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

set(FLATCC_INCLUDE_DIR ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/include)


# Targets
# =======

# Target - Diagnostic Codec
# -------------------------
add_library(diag-codec-impl OBJECT
    codec.c
    uds_fbs.c
    tp_fbs.c
    srpc_fbs.c
    xil_fbs.c
)
target_include_directories(diag-codec-impl
    PUBLIC
        ${FLATCC_INCLUDE_DIR}
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
)
target_link_libraries(diag-codec-impl
    PUBLIC
        ncodec-api
        $<TARGET_OBJECTS:ncodec-api>
        ncodec-stream
        $<TARGET_OBJECTS:ncodec-stream>
        ncodec-flatcc
        $<TARGET_OBJECTS:ncodec-flatcc>
)
