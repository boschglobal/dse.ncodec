# Copyright 2025 Robert Bosch GmbH
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

include(GNUInstallDirs)
include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/_out)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-g -ggdb")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O3")
list(APPEND C_CXX_WARNING_FLAGS
    -Wall
    -W
    -Wwrite-strings
    -Wno-missing-field-initializers
    -Wno-misleading-indentation
)
add_compile_options(${C_CXX_WARNING_FLAGS})
add_compile_definitions(DLL_BUILD)


set(REPO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
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
#FetchContent_Declare(dse_ncodec
#    SOURCE_DIR          "${CMAKE_CURRENT_SOURCE_DIR}/../ncodec"
#    SOURCE_SUBDIR       dse/ncodec
#)
#FetchContent_MakeAvailable(dse_ncodec)
#set(DSE_NCODEC_SOURCE_DIR ${dse_ncodec_SOURCE_DIR}/dse/ncodec)
#set(DSE_NCODEC_INCLUDE_DIR ${dse_ncodec_SOURCE_DIR})

# Associate Project - DSE Network Codec
# -------------------------------------
# Directly add the subdirectory to ensure targets are fully visible
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../ncodec" "${CMAKE_CURRENT_BINARY_DIR}/ncodec")
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
add_library(yaml STATIC IMPORTED GLOBAL)
set_target_properties(yaml
    PROPERTIES
        IMPORTED_LOCATION "${YAML_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${YAML_BINARY_DIR}"
)


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
add_library(lua STATIC IMPORTED GLOBAL)
set_target_properties(lua
    PROPERTIES
        IMPORTED_LOCATION "${LUA_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${LUA_BINARY_DIR}"
)


# Targets
# =======

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
    ${DSE_CLIB_SOURCE_DIR}/clib/collections/hashmap.c
    ${DSE_CLIB_SOURCE_DIR}/clib/collections/set.c
    ${DSE_CLIB_SOURCE_DIR}/clib/util/binary.c
    ${DSE_CLIB_SOURCE_DIR}/clib/data/marshal.c
    ${DSE_CLIB_SOURCE_DIR}/clib/util/yaml.c
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
add_subdirectory(examples EXCLUDE_FROM_ALL)


# Common Targets
# ==============
install(
    DIRECTORY
        ${REPO_DIR}/licenses/
    DESTINATION
        licenses
    COMPONENT
        licenses
)
install(
    FILES
        ${CMAKE_BINARY_DIR}/compile_commands.json
    DESTINATION
        doc
    COMPONENT
        doc
)


# Package
# =======
set(CPACK_SYSTEM_NAME "$ENV{PACKAGE_ARCH}")
set(CPACK_PACKAGE_VENDOR "Robert Bosch GmbH")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DSE PDU Network")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/_dist")
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()
set(CPACK_PACKAGE_FILE_NAME
    "PDUNet-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}"
)
set(CPACK_COMPONENTS_ALL
    pdunet
    licenses
    doc
)

include(CPack)
