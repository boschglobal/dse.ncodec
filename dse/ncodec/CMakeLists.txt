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
if(DSE_NCODEC_BUILD_EXAMPLES)
    add_subdirectory(examples EXCLUDE_FROM_ALL)
endif()


# Package
# =======
set(CPACK_SYSTEM_NAME "$ENV{PACKAGE_ARCH}")
set(CPACK_PACKAGE_VENDOR "Robert Bosch GmbH")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "DSE Network Codec")
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
    "NCodec-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}"
)
set(CPACK_COMPONENTS_ALL
    abcodec
    licenses
    doc
)

include(CPack)
