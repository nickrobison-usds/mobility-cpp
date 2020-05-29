find_package(GDAL REQUIRED)

# Find HDF5
set(HDF5_PREFER_PARALLEL True)

find_package(HDF5 REQUIRED COMPONENTS CXX HL)

if (NOT HDF5_IS_PARALLEL)
    message(FATAL_ERROR "HDF5 C++ library must be thread-safe")
endif ()

add_library(io src/parquet.cpp include/io/parquet.hpp src/shapefile.cpp include/io/shapefile.hpp include/io/csv_reader.hpp include/io/csv.hpp src/hdf5.cpp include/io/hdf5.hpp)
target_link_libraries(io PUBLIC arrow parquet GDAL hdf5::hdf5-static hdf5::hdf5_hl-static PRIVATE spdlog)
target_include_directories(io PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_executable(testio test/shapefile_test.cpp test/csv_test.cpp test/hdf_test.cpp)
target_link_libraries(testio Catch2::Catch2 io Boost::filesystem)
add_test(testio testio WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/io/test)