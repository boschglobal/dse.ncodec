# Copyright 2025 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

set(MODULE "AB Codec")
set(MODULE_LC "abcodec")
project(${MODULE}
    VERSION "$ENV{PACKAGE_VERSION}"
    DESCRIPTION "Dynamic Simulation Environment - AB Codec"
    HOMEPAGE_URL "$ENV{PROJECT_URL}"
)
include(GNUInstallDirs)
set(INSTALL_SUBDIR ${MODULE_LC})
# set(CMAKE_VERBOSE_MAKEFILE ON)


add_subdirectory(ab)


# External Project - dlfcn-win32
# ------------------------------
if(WIN32)
    set(DLFCNWIN32_SOURCE_DIR "$ENV{EXTERNAL_BUILD_DIR}/dlfcnwin32/src")
    set(DLFCNWIN32_BINARY_DIR "$ENV{EXTERNAL_BUILD_DIR}/dlfcnwin32-build/lib")
    find_library(DLFCNWIN32_LIB
        NAMES
            libdl.a
        PATHS
            ${DLFCNWIN32_BINARY_DIR}
        REQUIRED
        NO_DEFAULT_PATH
    )
    if(NOT TARGET dse_ncodec_dl)
        add_library(dse_ncodec_dl STATIC IMPORTED GLOBAL)
        set_target_properties(dse_ncodec_dl
            PROPERTIES
                IMPORTED_LOCATION "${DLFCNWIN32_LIB}"
                INTERFACE_INCLUDE_DIRECTORIES "${DLFCNWIN32_SOURCE_DIR}"
        )
    endif()
    set(DSE_NCODEC_DL_TARGET dse_ncodec_dl)
else()
    set(DSE_NCODEC_DL_TARGET ${CMAKE_DL_LIBS})
endif()


# Targets
# =======


# Target - Automotive Bus Codec Library
# -------------------------------------
add_library(ab-codec OBJECT
    ncodec.c
)
target_link_libraries(ab-codec
    PUBLIC
        ab-codec-impl
        $<TARGET_OBJECTS:ab-codec-impl>
        ${DSE_NCODEC_DL_TARGET}
)
add_library(ab-codec-shared-lib SHARED)
target_link_libraries(ab-codec-shared-lib
    PUBLIC
        ab-codec
)
set_target_properties(ab-codec-shared-lib
    PROPERTIES
        OUTPUT_NAME ${MODULE_LC}
)
install(
    TARGETS
        ab-codec-shared-lib
    LIBRARY DESTINATION
        ${INSTALL_SUBDIR}/lib
    RUNTIME DESTINATION
        ${INSTALL_SUBDIR}/lib
    COMPONENT
        ${MODULE_LC}
)
install(
    FILES
        ${DSE_NCODEC_SOURCE_DIR}/codec.h
        ${DSE_NCODEC_SOURCE_DIR}/interface/frame.h
        ${DSE_NCODEC_SOURCE_DIR}/interface/pdu.h
    DESTINATION
        ${INSTALL_SUBDIR}/${CMAKE_INSTALL_INCLUDEDIR}/dse/ncodec
    COMPONENT
        ${MODULE_LC}
)
install(
    FILES
        ${DSE_NCODEC_SOURCE_DIR}/stream/stream.h
    DESTINATION
        ${INSTALL_SUBDIR}/${CMAKE_INSTALL_INCLUDEDIR}/dse/ncodec/stream
    COMPONENT
        ${MODULE_LC}
 )
