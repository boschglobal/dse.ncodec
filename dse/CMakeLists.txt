# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0135 NEW)

project(DSE
    VERSION "$ENV{PACKAGE_VERSION}"
    DESCRIPTION "Dynamic Simulation Environment - Network Codec"
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
set(REPO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

option(DSE_NCODEC_BUILD_EXAMPLES "Build DSE NCodec examples" ${PROJECT_IS_TOP_LEVEL})

# Sub Modules
# ===========
add_subdirectory(ncodec)
add_subdirectory(pdunet)


# Example Targets
# ===============
if(PROJECT_IS_TOP_LEVEL)
    add_custom_target(examples
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}/ncodec/examples" --parallel
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}/pdunet/examples" --parallel
        COMMENT "Building ncodec and pdunet examples"
    )

    add_custom_target(install-examples
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}/ncodec/examples" --target install --parallel
        COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}/pdunet/examples" --target install --parallel
        DEPENDS examples
        COMMENT "Installing ncodec and pdunet examples"
    )
endif()


# Administrative Targets
# ======================
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
