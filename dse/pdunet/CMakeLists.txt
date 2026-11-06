# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0135 NEW)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(VERSION "$ENV{PACKAGE_VERSION}")

project(PduNet
    VERSION ${VERSION}
    DESCRIPTION "Dynamic Simulation Environment - PDU Network"
    HOMEPAGE_URL "$ENV{PROJECT_URL}"
)

set(DSE_PDUNET_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(DSE_PDUNET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")


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
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../ncodec" "${CMAKE_CURRENT_BINARY_DIR}/ncodec")
endif()
set(DSE_NCODEC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../ncodec")
set(DSE_NCODEC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")


# External Projects - yaml, lua
# -----------------------------
include(${CMAKE_CURRENT_SOURCE_DIR}/../../extra/external/yaml.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../extra/external/lua.cmake)

# Targets
# =======

set(PDUNET_CLIB_SOURCES
    ${DSE_CLIB_SOURCE_DIR}/clib/collections/hashmap.c
    ${DSE_CLIB_SOURCE_DIR}/clib/collections/set.c
    ${DSE_CLIB_SOURCE_DIR}/clib/util/binary.c
    ${DSE_CLIB_SOURCE_DIR}/clib/data/marshal.c
    ${DSE_CLIB_SOURCE_DIR}/clib/util/yaml.c
)


# Target - Automotive Bus Codec
# -----------------------------
add_library(pdunet OBJECT
    pdunet.c
    network/can.c
    network/flexray.c
    network/lua.c
    network/matrix.c
    network/network.c
    schema.c
)
target_include_directories(pdunet
    PUBLIC
        ${DSE_PDUNET_INCLUDE_DIR}
        ${DSE_NCODEC_INCLUDE_DIR}
        ${DSE_CLIB_INCLUDE_DIR}
)
target_link_libraries(pdunet
    PUBLIC
        dse_external_yaml
        dse_external_lua
)


# Sub Modules
# ===========
if(DSE_NCODEC_BUILD_EXAMPLES)
    add_subdirectory(examples EXCLUDE_FROM_ALL)
endif()
