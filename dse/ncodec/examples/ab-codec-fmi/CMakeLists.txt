# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(AB_Codec_FMI_Example)
set(TARGET_NAME "ab-codec-fmi-example")
set(EXAMPLE_PATH "examples/ab-codec-fmi")

add_executable(${TARGET_NAME}
    main.c
    ncodec.c
    fmu2.c
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
