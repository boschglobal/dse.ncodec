# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(Codec_Example)
set(EXAMPLE_PATH "examples/codec")


# Targets
# =======
add_library(codec
    SHARED
    codec.c
)
target_include_directories(codec
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
)
install(TARGETS codec
    LIBRARY DESTINATION
        ${EXAMPLE_PATH}/lib
    RUNTIME DESTINATION
        ${EXAMPLE_PATH}/lib
)


add_executable(example_dynamic
    dynamic.c
    example.c
    ${DSE_NCODEC_SOURCE_DIR}/codec.c
    ${DSE_NCODEC_SOURCE_DIR}/stream/buffer.c
)
target_include_directories(example_dynamic
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
)
target_link_libraries(example_dynamic
    PRIVATE
        dse_external_dl
)
set_target_properties(example_dynamic PROPERTIES
    INSTALL_RPATH "$ORIGIN"
)
install(
    TARGETS
        example_dynamic
    DESTINATION
        ${EXAMPLE_PATH}/bin
)


add_executable(example_static
    codec.c
    example.c
    static.c
    ${DSE_NCODEC_SOURCE_DIR}/codec.c
    ${DSE_NCODEC_SOURCE_DIR}/stream/buffer.c
)
target_include_directories(example_static
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
)
install(
    TARGETS
        example_static
    DESTINATION
        ${EXAMPLE_PATH}/bin
)
