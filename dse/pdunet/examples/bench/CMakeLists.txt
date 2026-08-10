cmake_minimum_required(VERSION 3.21)

project(PDUNet_Benchmark)

set(EXAMPLE_PATH "examples/bench")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

set(BENCH_SOURCES
    bench.c
    main.c
)

add_executable(bench
    ${BENCH_SOURCES}
)
target_link_libraries(bench
    PRIVATE
        pdunet
        ab-codec
        yaml
        lua
        m
)
add_executable(bench_profile
    ${BENCH_SOURCES}
)
target_link_libraries(bench_profile
    PRIVATE
        pdunet
        ab-codec
        yaml
        lua
        m
)

target_compile_options(bench_profile
    PRIVATE
        -pg
        -fno-omit-frame-pointer
)

target_link_options(bench_profile
    PRIVATE
        -pg
)


install(
    TARGETS
        bench
        bench_profile
    DESTINATION
        ${EXAMPLE_PATH}/bin
)

install(
    FILES
        network.yaml
    DESTINATION
        ${EXAMPLE_PATH}/data
)
