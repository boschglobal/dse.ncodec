cmake_minimum_required(VERSION 3.21)

project(PDUNet_Example)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

add_executable(pdunet_example
    example.c
    main.c
    ${PDUNET_CLIB_SOURCES}
)

target_link_libraries(pdunet_example
    PRIVATE
        pdunet
        ab-codec
        ${DSE_NCODEC_YAML_TARGET}
        ${DSE_NCODEC_LUA_TARGET}
        m
        $<$<BOOL:${WIN32}>:ws2_32>
        $<$<BOOL:${WIN32}>:iphlpapi>
        $<$<AND:$<BOOL:${WIN32}>,$<STREQUAL:${CMAKE_CXX_COMPILER_ID},"GNU">>:"-static winpthread">
)
