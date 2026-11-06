# Copyright 2026 Robert Bosch GmbH
#
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)
if(TARGET dse_external_ffi)
    return()
endif()

include(ExternalProject)
include(${CMAKE_CURRENT_LIST_DIR}/dse_external.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/oss_repos.cmake)
if(DEFINED ENV{REPO_MIRRORS})
    include($ENV{REPO_MIRRORS} OPTIONAL)
endif()

set(DSE_EXTERNAL_FFI_PREFIX "${CMAKE_BINARY_DIR}/_deps/dse_external_ffi")
set(DSE_EXTERNAL_FFI_INSTALL_DIR "${DSE_EXTERNAL_FFI_PREFIX}/install")
set(DSE_EXTERNAL_FFI_LIB "${DSE_EXTERNAL_FFI_INSTALL_DIR}/lib/libffi.a")
set(DSE_EXTERNAL_FFI_INCLUDE_DIR "${DSE_EXTERNAL_FFI_INSTALL_DIR}/include")

set(DSE_EXTERNAL_FFI_CONFIGURE_HOST "")
if(DEFINED TOOLCHAIN_PREFIX)
    set(DSE_EXTERNAL_FFI_CONFIGURE_HOST --host=${TOOLCHAIN_PREFIX})
elseif(CMAKE_C_FLAGS MATCHES "(^| )-mx32( |$)")
    # Not all toolchain files set TOOLCHAIN_PREFIX; without --host configure runs x32 test binaries.
    set(DSE_EXTERNAL_FFI_CONFIGURE_HOST --host=x86_64-linux-gnux32)
elseif(CMAKE_C_FLAGS MATCHES "(^| )-m32( |$)")
    set(DSE_EXTERNAL_FFI_CONFIGURE_HOST --host=i686-linux-gnu)
endif()

dse_external_download(ffi "${ExternalProject__FFI__URL}"
    "${ExternalProject__FFI__USERNAME}" "${ExternalProject__FFI__PASSWORD}" DSE_EXTERNAL_FFI_ARCHIVE)
ExternalProject_Add(dse_external_ffi_build
    URL                 "${DSE_EXTERNAL_FFI_ARCHIVE}"
    URL_HASH            ${DSE_EXTERNAL_FFI_ARCHIVE_HASH}
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    PREFIX              "${DSE_EXTERNAL_FFI_PREFIX}"
    INSTALL_DIR         "${DSE_EXTERNAL_FFI_INSTALL_DIR}"
    EXCLUDE_FROM_ALL    TRUE
    BUILD_IN_SOURCE     TRUE
    CONFIGURE_COMMAND   ./configure
                        ${DSE_EXTERNAL_FFI_CONFIGURE_HOST}
                        --prefix=<INSTALL_DIR>
                        CC=${CMAKE_C_COMPILER}
                        "CFLAGS=${CMAKE_C_FLAGS} -Wno-deprecated-declarations"
                        --disable-shared
                        --enable-static
                        --with-pic
                        --disable-docs
                        --disable-multi-os-directory
    BUILD_COMMAND       make
    INSTALL_COMMAND     make install LIBTOOLFLAGS=--silent
    BUILD_BYPRODUCTS    "${DSE_EXTERNAL_FFI_LIB}"
)

# Imported targets require their include directories to exist at generate time.
file(MAKE_DIRECTORY "${DSE_EXTERNAL_FFI_INCLUDE_DIR}")
add_library(dse_external_ffi STATIC IMPORTED GLOBAL)
set_target_properties(dse_external_ffi
    PROPERTIES
        IMPORTED_LOCATION "${DSE_EXTERNAL_FFI_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${DSE_EXTERNAL_FFI_INCLUDE_DIR}"
)
add_dependencies(dse_external_ffi dse_external_ffi_build)
