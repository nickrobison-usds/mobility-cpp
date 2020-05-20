

add_executable(benchmarks simple.cpp row_expansion.cpp)
target_link_libraries(benchmarks arrow benchmark::benchmark_main)