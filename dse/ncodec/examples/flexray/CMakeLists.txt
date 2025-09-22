# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(AB_Codec_FMI_Example)
set(EXAMPLE_PATH "examples/ab-codec-fmi")

set(BRIDGE_TARGET_NAME "bridge")
add_executable(${BRIDGE_TARGET_NAME}
    bridge.c
    ncodec.c
    flexray_anycpu.c
)
target_link_libraries(${BRIDGE_TARGET_NAME}
    PUBLIC
        ab-codec
)
target_include_directories(${BRIDGE_TARGET_NAME}
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
        .
)
install(TARGETS ${BRIDGE_TARGET_NAME}
    RUNTIME DESTINATION
        ${EXAMPLE_PATH}/
)



set(WUP_TARGET_NAME "wup")
add_executable(${WUP_TARGET_NAME}
    wup.c
    ncodec.c
    board_stub.c
    ecu_stub.c
    flexray_anycpu.c
)
target_link_libraries(${WUP_TARGET_NAME}
    PUBLIC
        ab-codec
)
target_include_directories(${WUP_TARGET_NAME}
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
        .
)
install(TARGETS ${WUP_TARGET_NAME}
    RUNTIME DESTINATION
        ${EXAMPLE_PATH}/
)



set(STARTUP_TARGET_NAME "startup")
add_executable(${STARTUP_TARGET_NAME}
    startup.c
    ncodec.c
    ecu_stub.c
    flexray_anycpu.c
)
target_link_libraries(${STARTUP_TARGET_NAME}
    PUBLIC
        ab-codec
)
target_include_directories(${STARTUP_TARGET_NAME}
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
        .
)
install(TARGETS ${STARTUP_TARGET_NAME}
    RUNTIME DESTINATION
        ${EXAMPLE_PATH}/
)



set(TXTR_TARGET_NAME "txrx")
add_executable(${TXTR_TARGET_NAME}
    txrx.c
    ncodec.c
    ecu_stub.c
    flexray_anycpu.c
)
target_link_libraries(${TXTR_TARGET_NAME}
    PUBLIC
        ab-codec
)
target_include_directories(${TXTR_TARGET_NAME}
    PRIVATE
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
        .
)
install(TARGETS ${TXTR_TARGET_NAME}
    RUNTIME DESTINATION
        ${EXAMPLE_PATH}/
)
