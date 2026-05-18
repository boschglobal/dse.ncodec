# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(AB_Codec_Examples)

set(TARGET_NAME "ab-cosim")
set(EXAMPLE_PATH "examples/ab-codec")
add_executable(${TARGET_NAME}
    cosim.c
    ncodec.c
)
target_link_libraries(${TARGET_NAME}
    PUBLIC
        ab-codec
)
target_include_directories(${TARGET_NAME}
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
        ${FLATCC_INCLUDE_DIR}
)
install(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION
        ${EXAMPLE_PATH}/
)
