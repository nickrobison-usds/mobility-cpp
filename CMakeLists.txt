cmake_minimum_required(VERSION 3.0.0)
project(mobility-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

set(DEBUG_FLAGS, "-O0 -g -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -Wall -Werror -Wpendantic")
set(RELEASE_FLAGS, "-O3")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(RELEASE_FLAGS ${RELEASE_FLAGS} "-march=native")
endif()
# Disable GTest dependency, since we use Catch
set(BENCHMARK_ENABLE_TESTING OFF)
#Set options for Blaze Library
set(BLAZE_SMP_THREADS HPX)
set(BLAZE_USE_VECTORIZATION TRUE)

enable_testing()

add_subdirectory(extern/spdlog)
add_subdirectory(extern/catch2)
add_subdirectory(extern/benchmark)
add_subdirectory(extern/blaze)
add_subdirectory(extern/abseil)
add_subdirectory(extern/date)

add_subdirectory(benchmarks)
add_subdirectory(tests)
add_subdirectory(locales)
add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

