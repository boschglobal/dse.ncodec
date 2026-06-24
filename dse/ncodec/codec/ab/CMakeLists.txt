# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

set(MODULE "AB Codec")
set(MODULE_LC "abcodec")
project(${MODULE}
    VERSION "$ENV{PACKAGE_VERSION}"
    DESCRIPTION "Dynamic Simulation Environment - AB Codec"
    HOMEPAGE_URL "$ENV{PROJECT_URL}"
)
include(GNUInstallDirs)
set(INSTALL_SUBDIR ${MODULE_LC})
set(FLATCC_SOURCE_DIR  ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/src)
set(FLATCC_INCLUDE_DIR ${DSE_NCODEC_SOURCE_DIR}/schema/abs/flatcc/include)
# set(CMAKE_VERBOSE_MAKEFILE ON)


# Targets
# =======

# Target - Automotive Bus Codec
# -----------------------------
add_library(ab-codec OBJECT
        codec.c
        frame_fbs.c
        pdu_fbs.c
        flexray/engine.c
        flexray/fbs.c
        flexray/state.c
        flexray/flexray.c
        flexray_pop/flexray_pop.c
        ${DSE_NCODEC_SOURCE_DIR}/codec.c
        ${DSE_NCODEC_SOURCE_DIR}/stream/ascii85.c
        ${DSE_NCODEC_SOURCE_DIR}/stream/buffer.c
        ${FLATCC_SOURCE_DIR}/builder.c
        ${FLATCC_SOURCE_DIR}/emitter.c
        ${FLATCC_SOURCE_DIR}/refmap.c
)
target_include_directories(ab-codec
    PRIVATE
        ${FLATCC_INCLUDE_DIR}
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
)


# Target - Automotive Bus Codec Library
# -------------------------------------
add_library(ab-codec-shared-lib SHARED)
target_link_libraries(ab-codec-shared-lib
    PUBLIC
        ab-codec
)
set_target_properties(ab-codec-shared-lib
    PROPERTIES
        OUTPUT_NAME ${MODULE_LC}
)
install(
    TARGETS
        ab-codec-shared-lib
    LIBRARY DESTINATION
        ${INSTALL_SUBDIR}/lib
    RUNTIME DESTINATION
        ${INSTALL_SUBDIR}/lib
    COMPONENT
        ${MODULE_LC}
)
install(
    FILES
        ${DSE_NCODEC_SOURCE_DIR}/codec.h
        ${DSE_NCODEC_SOURCE_DIR}/interface/frame.h
        ${DSE_NCODEC_SOURCE_DIR}/interface/pdu.h
    DESTINATION
        ${INSTALL_SUBDIR}/${CMAKE_INSTALL_INCLUDEDIR}/dse/ncodec
    COMPONENT
        ${MODULE_LC}
)
install(
    FILES
        ${DSE_NCODEC_SOURCE_DIR}/stream/stream.h
    DESTINATION
        ${INSTALL_SUBDIR}/${CMAKE_INSTALL_INCLUDEDIR}/dse/ncodec/stream
    COMPONENT
        ${MODULE_LC}
 )
