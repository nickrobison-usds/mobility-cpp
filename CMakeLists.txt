cmake_minimum_required(VERSION 3.0.0)

# ##############################################################################
# Build type (needs to be handled before project command below)
# ##############################################################################
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
            "Release"
            CACHE
            STRING
            "Configuration type (one of Debug, RelWithDebInfo, Release, MinSizeRel)"
            FORCE
            )
endif ()

project(mobility-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake "${CMAKE_SOURCE_DIR}/extern/sanitizers-cmake/cmake")

set(DEBUG_FLAGS, "-O0 -g -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -Wall -Werror -Wpendantic")
set(RELEASE_FLAGS, "-O3")

include(CheckCXXCompilerFlag)
include(ExternalProject)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if (COMPILER_SUPPORTS_MARCH_NATIVE)
    set(RELEASE_FLAGS ${RELEASE_FLAGS} "-march=native")
    set(DEBUG_FLAGS ${DEBUG_FLAGS} "-march=native")
endif ()

find_package(Sanitizers)

# Disable GTest dependency, since we use Catch
set(BENCHMARK_ENABLE_TESTING OFF)
set(BLAZE_USE_VECTORIZATION TRUE)

# Set the default malloc implementation
set(DEFAULT_MALLOC "system" CACHE STRING "Set the default memory allocator")
set_property(CACHE DEFAULT_MALLOC PROPERTY STRINGS "system;tcmalloc;jemalloc")

set(RANGES_CXX_STD 17 CACHE STRING "")

# Whether or not we should build HPX ourselves
set(USE_INTERNAL_HPX OFF CACHE BOOL "Build the bundled version of HPX")
set(USE_INTERNAL_HDF5 OFF CACHE BOOL "Build the bundled version of HDF5")
set(USE_INTERNAL_ARROW OFF CACHE BOOL "Build the bundled version of Apache Arrow")
set(CI OFF CACHE BOOL "Run using CI configuration")
set(JSON_BuildTests OFF CACHE INTERNAL "")

if (${CI})
    set(USE_INTERNAL_HPX ON)
    set(USE_INTERNAL_HDF5 ON)
    set(USE_INTERNAL_ARROW ON)
    # Disable vectorization due to AVX2 issues
    set(BLAZE_USE_VECTORIZATION OFF CACHE BOOL "")
    # Enable Metal builds on MacOS
    if (APPLE)
        set(USE_METAL ON)
    endif ()
endif ()

if (NOT ${USE_INTERNAL_HPX})
    #Set options for Blaze Library
    set(BLAZE_SMP_THREADS HPX)
endif ()

set(USE_METAL OFF CACHE BOOL "Enable Apple Metal support")

if (${USE_METAL})
    message(INFO "Enabling Apple Metal support")
    add_compile_definitions(USE_METAL)
endif ()

enable_testing()

add_subdirectory(extern/spdlog)
add_subdirectory(extern/catch2)
add_subdirectory(extern/benchmark)
add_subdirectory(extern/blaze)
add_subdirectory(extern/abseil)
add_subdirectory(extern/date)
add_subdirectory(extern/yaml-cpp)
add_subdirectory(extern/range-v3)
add_subdirectory(extern/pybind11)
add_subdirectory(extern/xtl)
add_subdirectory(extern/xtensor)
add_subdirectory(extern/xtensor-python)
add_subdirectory(extern/jsoon)

if (${USE_INTERNAL_HPX})
    message("Using internal HPX")
    set(HPX_USE_CMAKE_CXX_STANDARD ON CACHE BOOL "")
    set(HPX_WITH_MALLOC "system" CACHE STRING "")
    set(HPX_WITH_EXAMPLES OFF CACHE BOOL "")
    set(HPX_WITH_TESTS OFF CACHE BOOL "")
    add_subdirectory(extern/hpx)
endif ()

if (${USE_INTERNAL_HDF5})
    message("Using internal HDF5")
    set(HDF5_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/hdf5")
    set(HDF5_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/hdf5")
    set(HDF5_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${HDF5_INSTALL_DIR} -DHDF5_BUILD_TOOLS=OFF -DHDF5_BUILD_UTILS=OFF -DHDF5_BUILD_EXAMPLES=OFF -DBUILD_TESTING=OFF)
    ExternalProject_Add(hdf5_ep
            PREFIX ${HDF5_PREFIX}
            URL "https://github.com/HDFGroup/hdf5/archive/hdf5-1_12_0.tar.gz"
            URL_HASH SHA256=c64ffec2539ae6b6041f97952a40b0893c3c0df4d5b1c0177fb8aba567808158
            INSTALL_DIR ${HDF5_INSTALL_DIR}
            CMAKE_ARGS ${HDF5_CMAKE_ARGS}
            CMAKE_GENERATOR "Unix Makefiles")
endif ()

if (${USE_INTERNAL_ARROW})
    #    find_package(Thrift REQUIRED)
    message("Using internal Apache Arrow")
    set(ARROW_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/arrow")
    set(ARROW_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/arrow")
    set(ARROW_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${ARROW_INSTALL_DIR} -DARROW_PARQUET=ON -DARROW_JEMALLOC=OFF -DARROW_BUILD_STATIC=ON -DARROW_DEPENDENCY_SOURCE=AUTO)
    ExternalProject_Add(arrow_ep
            PREFIX ${ARROW_PREFIX}
            URL "https://www.apache.org/dist/arrow/arrow-0.17.1/apache-arrow-0.17.1.tar.gz"
            URL_HASH SHA256=cbc51c343bca08b10f7f1b2ef15cb15057c30e5e9017cfcee18337b7e2da9ea2
            SOURCE_SUBDIR cpp
            INSTALL_DIR ${ARROW_INSTALL_DIR}
            CMAKE_ARGS ${ARROW_CMAKE_ARGS}
            CMAKE_GENERATOR "Unix Makefiles")
endif ()

add_subdirectory(tests)
add_subdirectory(locales)
add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Custom target
add_custom_target(applications DEPENDS connectivity distance locales)

