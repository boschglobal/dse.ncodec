# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

# set(CMAKE_VERBOSE_MAKEFILE ON)
set(VERSION "$ENV{PACKAGE_VERSION}")

project(DiagNet
    VERSION ${VERSION}
    DESCRIPTION "Dynamic Simulation Environment - Diagnostic Network"
    HOMEPAGE_URL "$ENV{PROJECT_URL}"
)

# Sub Modules
# ===========
add_subdirectory(service)
if(DSE_NCODEC_BUILD_EXAMPLES)
    add_subdirectory(examples EXCLUDE_FROM_ALL)
endif()
