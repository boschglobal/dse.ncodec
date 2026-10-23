# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(DiagNet_Plant_Example)

set(EXAMPLE_PATH "examples/diagnet/plant")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_executable(plant_example
    plant.c
    example.c
    main.c
)
set_target_properties(plant_example
    PROPERTIES
        OUTPUT_NAME "plant"
)
target_link_libraries(plant_example
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
        plant_example
    DESTINATION
        ${EXAMPLE_PATH}/bin
)
