# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0135 NEW)

project(DSE_CLIB__External_Projects)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS_DEBUG "-g -ggdb")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O3")
add_compile_options(-Wall -W -Wwrite-strings -Wno-missing-field-initializers)

include(ExternalProject)
include(FetchContent)


# Load URLs
# =========
include(oss_repos.cmake)
if(DEFINED ENV{REPO_MIRRORS})
    include($ENV{REPO_MIRRORS} OPTIONAL)
endif()


# External Projects
# =================
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS download)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")


# Automotive Bus Schema
# ---------------------
FetchContent_Declare(abs
    URL                 $ENV{ABS_URL}
    HTTP_USERNAME       $ENV{ABS_URL_USER}
    HTTP_PASSWORD       $ENV{ABS_URL_TOKEN}
    SOURCE_DIR          "$ENV{EXTERNAL_BUILD_DIR}/automotive-bus-schema"
)
FetchContent_MakeAvailable(abs)


# DSE C Lib
# ---------
FetchContent_Declare(dse_clib
    URL                 $ENV{DSE_CLIB_URL}
    HTTP_USERNAME       $ENV{DSE_CLIB_URL_USER}
    HTTP_PASSWORD       $ENV{DSE_CLIB_URL_TOKEN}
    SOURCE_DIR          "$ENV{EXTERNAL_BUILD_DIR}/dse.clib"
)
FetchContent_MakeAvailable(dse_clib)



# OSS Target
# ==========
add_custom_target(oss)
function(oss_copy src dst)
    add_custom_command(TARGET oss PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${src}"
            "../../../dse/__oss__/${dst}"
    )
endfunction()
# add_dependencies(oss)
oss_copy(${dse_clib_SOURCE_DIR} boschglobal/automotive-bus-schema)
oss_copy(${dse_clib_SOURCE_DIR} boschglobal/dse.clib)
