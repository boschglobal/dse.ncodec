# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(DiagNet_Rpn_Example)

set(EXAMPLE_PATH "examples/diagnet/rpn")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_executable(rpn_example
    example.c
    main.c
    rpn.c
)
set_target_properties(rpn_example
    PROPERTIES
        OUTPUT_NAME "rpn"
)
target_link_libraries(rpn_example
    PRIVATE
        diagnet_service
        ab-codec
        dse_ncodec_ffi
        m
        $<$<BOOL:${WIN32}>:ws2_32>
        $<$<BOOL:${WIN32}>:iphlpapi>
        $<$<AND:$<BOOL:${WIN32}>,$<STREQUAL:${CMAKE_CXX_COMPILER_ID},"GNU">>:"-static winpthread">
)


install(
    TARGETS
        rpn_example
    DESTINATION
        ${EXAMPLE_PATH}/bin
)
