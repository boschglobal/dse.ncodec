# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

project(PduNet_Examples
    VERSION ${VERSION}
    DESCRIPTION "Dynamic Simulation Environment - PDU Network - Examples"
    HOMEPAGE_URL "$ENV{PROJECT_URL}"
)

# Example Models.
add_subdirectory(pdunet)
if(UNIX)
add_subdirectory(bench)
endif()