# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0135 NEW)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(VERSION "$ENV{PACKAGE_VERSION}")

project(DiagNetService
    VERSION ${VERSION}
    DESCRIPTION "Dynamic Simulation Environment - Diagnostic Network - Service"
    HOMEPAGE_URL "$ENV{PROJECT_URL}"
)

set(DSE_DIAGNET_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(DSE_DIAGNET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../..")


# External Project - DSE C Lib
# ----------------------------
FetchContent_Declare(dse_clib
    URL                 $ENV{DSE_CLIB_URL}
    HTTP_USERNAME       $ENV{DSE_CLIB_URL_USER}
    HTTP_PASSWORD       $ENV{DSE_CLIB_URL_TOKEN}
    SOURCE_DIR          "$ENV{EXTERNAL_BUILD_DIR}/dse.clib"
)
FetchContent_MakeAvailable(dse_clib)
set(DSE_CLIB_SOURCE_DIR "${dse_clib_SOURCE_DIR}/dse")
set(DSE_CLIB_INCLUDE_DIR "${DSE_CLIB_SOURCE_DIR}/..")


# Associate Project - DSE Network Codec
# -------------------------------------
# Directly add the subdirectory to ensure targets are fully visible
if(NOT TARGET ab-codec-shared-lib)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../ncodec" "${CMAKE_CURRENT_BINARY_DIR}/ncodec")
endif()
set(DSE_NCODEC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../ncodec")
set(DSE_NCODEC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../..")


# External Project - ffi
# ----------------------
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../extra/external/ffi.cmake)


# Targets
# =======

set(DIAGNET_SERVICE_CLIB_SOURCES
#    ${DSE_CLIB_SOURCE_DIR}/clib/collections/.c
)


# Target - DIAG Net Service
# -----------------------------
add_library(diagnet_service OBJECT
    service.c
    client.c
    call.c
    ${DIAGNET_SERVICE_CLIB_SOURCES}
)
target_include_directories(diagnet_service
    PUBLIC
        ${DSE_DIAGNET_INCLUDE_DIR}
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
)
target_link_libraries(diagnet_service
    PUBLIC
        dse_external_ffi
        ncodec-flatcc
        $<TARGET_OBJECTS:ncodec-flatcc>
)
