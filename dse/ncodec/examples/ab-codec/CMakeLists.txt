# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(AB_Codec_Examples)

set(TARGET_AB_PDU "ab-pdu-cosim")
set(EXAMPLE_PATH "examples/ab-codec")
add_executable(${TARGET_AB_PDU}
    pdu_cosim.c
    ncodec.c
)
target_link_libraries(${TARGET_AB_PDU}
    PUBLIC
        ab-codec
)
install(TARGETS ${TARGET_AB_PDU}
    RUNTIME DESTINATION
        ${EXAMPLE_PATH}/
)

set(TARGET_AB_FRAY "ab-fray-cosim")
set(EXAMPLE_PATH "examples/ab-codec")
add_executable(${TARGET_AB_FRAY}
    fray_cosim.c
    fray_config.c
    ncodec.c
    simbus.c
)
target_link_libraries(${TARGET_AB_FRAY}
    PUBLIC
        ab-codec
)
install(TARGETS ${TARGET_AB_FRAY}
    RUNTIME DESTINATION
        ${EXAMPLE_PATH}/
)
