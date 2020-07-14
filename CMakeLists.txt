cmake_minimum_required(VERSION 3.0.0)

# ##############################################################################
# Build type (needs to be handled before project command below)
# ##############################################################################
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
            "Release"
            CACHE
            STRING
            "Configuration type (one of Debug, RelWithDebInfo, Release, MinSizeRel)"
            FORCE
            )
endif()

project(mobility-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake "${CMAKE_SOURCE_DIR}/extern/sanitizers-cmake/cmake")

set(DEBUG_FLAGS, "-O0 -g -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -Wall -Werror -Wpendantic")
set(RELEASE_FLAGS, "-O3")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(RELEASE_FLAGS ${RELEASE_FLAGS} "-march=native")
    set(DEBUG_FLAGS ${DEBUG_FLAGS} "-march=native")
endif()

find_package(Sanitizers)

# Set options for external dependencies

# Disable GTest dependency, since we use Catch
set(BENCHMARK_ENABLE_TESTING OFF)
#Set options for Blaze Library
set(BLAZE_SMP_THREADS HPX CACHE STRING "Use HPX threads")
set(BLAZE_USE_VECTORIZATION ON CACHE BOOL "Vectorize")
set(HDF5_ENABLE_PARALLEL ON CACHE BOOL "HDF5 Parallel")
set(HDF5_BUILD_CPP_LIB OFF CACHE BOOL "No HDF5 CPP lib")

# Set the default malloc implementation
set(DEFAULT_MALLOC "system" CACHE STRING "Set the default memory allocator")
set_property(CACHE DEFAULT_MALLOC PROPERTY STRINGS "system;tcmalloc")

enable_testing()

add_subdirectory(extern/spdlog)
add_subdirectory(extern/catch2)
add_subdirectory(extern/benchmark)
add_subdirectory(extern/blaze)
add_subdirectory(extern/abseil)
add_subdirectory(extern/date)
add_subdirectory(extern/hdf5)

add_subdirectory(benchmarks)
add_subdirectory(tests)
add_subdirectory(locales)
add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

