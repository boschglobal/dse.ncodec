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
if(NOT TARGET ab-codec)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../ncodec" "${CMAKE_CURRENT_BINARY_DIR}/ncodec")
endif()
set(DSE_NCODEC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../ncodec")
set(DSE_NCODEC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")


# External Project - yaml
# -----------------------
set(YAML_SOURCE_DIR "$ENV{EXTERNAL_BUILD_DIR}/yaml")
set(YAML_BINARY_DIR "$ENV{EXTERNAL_BUILD_DIR}/yaml")
find_library(YAML_LIB
    NAMES
        libyaml.a
    PATHS
        ${YAML_BINARY_DIR}
    REQUIRED
    NO_DEFAULT_PATH
)
if(NOT TARGET dse_ncodec_yaml)
    add_library(dse_ncodec_yaml STATIC IMPORTED GLOBAL)
    set_target_properties(dse_ncodec_yaml
        PROPERTIES
            IMPORTED_LOCATION "${YAML_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${YAML_BINARY_DIR}"
    )
endif()
set(DSE_NCODEC_YAML_TARGET dse_ncodec_yaml)


# External Project - lua
# ----------------------
set(LUA_SOURCE_DIR "$ENV{EXTERNAL_BUILD_DIR}/lua")
set(LUA_BINARY_DIR "$ENV{EXTERNAL_BUILD_DIR}/lua")
find_library(LUA_LIB
    NAMES
        liblua.a
    PATHS
        ${LUA_BINARY_DIR}
    REQUIRED
    NO_DEFAULT_PATH
)
if(NOT TARGET dse_ncodec_lua)
    add_library(dse_ncodec_lua STATIC IMPORTED GLOBAL)
    set_target_properties(dse_ncodec_lua
        PROPERTIES
            IMPORTED_LOCATION "${LUA_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${LUA_BINARY_DIR}"
    )
endif()
set(DSE_NCODEC_LUA_TARGET dse_ncodec_lua)

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
        ${LUA_SOURCE_DIR}
        ${YAML_SOURCE_DIR}/include
)


# Sub Modules
# ===========
if(DSE_NCODEC_BUILD_EXAMPLES)
    add_subdirectory(examples EXCLUDE_FROM_ALL)
endif()
