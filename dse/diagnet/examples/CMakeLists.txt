# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

project(DiagNet_Examples
    VERSION ${VERSION}
    DESCRIPTION "Dynamic Simulation Environment - Diagnostic Network - Examples"
    HOMEPAGE_URL "$ENV{PROJECT_URL}"
)

# Example Models.
if(UNIX)
add_subdirectory(rpn)
endif()
add_subdirectory(plant)