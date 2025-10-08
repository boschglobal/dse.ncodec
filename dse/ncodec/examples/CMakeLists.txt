# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

project(NCodec_Examples
    VERSION ${VERSION}
    DESCRIPTION "Dynamic Simulation Environment - Network Codec Library - Examples"
    HOMEPAGE_URL "$ENV{PROJECT_URL}"
)

# Example Models.
add_subdirectory(codec)
add_subdirectory(ab-codec-fmi)
add_subdirectory(flexray)
