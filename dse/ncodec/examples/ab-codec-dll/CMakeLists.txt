# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(AB_Codec_DLL_Example)
set(EXAMPLE_PATH "examples/ab-codec-dll")

add_library(ab-codec-dll SHARED
    codec.c
)
target_compile_definitions(ab-codec-dll PRIVATE DLL_BUILD)
target_link_libraries(ab-codec-dll
    PRIVATE
        ab-codec-impl
)
set_target_properties(ab-codec-dll
    PROPERTIES
        PREFIX ""
)
install(TARGETS ab-codec-dll
    LIBRARY DESTINATION ${EXAMPLE_PATH}/lib
    RUNTIME DESTINATION ${EXAMPLE_PATH}/lib
)

add_executable(ab-codec-dll-example
    main.c
)
target_link_libraries(ab-codec-dll-example PRIVATE ab-codec)
add_dependencies(ab-codec-dll-example ab-codec-dll)
install(TARGETS ab-codec-dll-example
    RUNTIME DESTINATION ${EXAMPLE_PATH}
)