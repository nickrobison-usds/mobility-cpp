

add_executable(benchmarks simple.cpp row_expansion.cpp)
target_link_libraries(benchmarks arrow benchmark::benchmark_main)
target_compile_options(benchmarks PRIVATE ${RELEASE_FLAGS})