# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0135 NEW)

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(VERSION "$ENV{PACKAGE_VERSION}")

project(NCodec
    VERSION ${VERSION}
    DESCRIPTION "Dynamic Simulation Environment - Network Codec Library"
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
set(DSE_NCODEC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(DSE_NCODEC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")


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


# Sub Modules
# ===========
add_subdirectory(codec/ab)
add_subdirectory(examples EXCLUDE_FROM_ALL)


# Common Targets
# ==============
install(
    DIRECTORY
        ${REPO_DIR}/licenses/
    DESTINATION
        licenses
)
install(
    FILES
        ${CMAKE_BINARY_DIR}/compile_commands.json
    DESTINATION
        doc
)
