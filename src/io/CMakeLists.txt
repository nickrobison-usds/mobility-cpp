find_package(GDAL REQUIRED)

# Find HDF5
set(HDF5_PREFER_PARALLEL False)

find_package(HDF5 REQUIRED COMPONENTS HL)

add_library(io src/parquet.cpp include/io/parquet.hpp src/Shapefile.cpp include/io/Shapefile.hpp include/io/csv_reader.hpp include/io/csv.hpp include/io/hdf5.hpp)
target_link_libraries(io PUBLIC arrow parquet gdal ${HDF5_C_LIBRARIES} ${HDF5_C_HL_LIBRARIES} PRIVATE spdlog)
target_include_directories(io PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${HDF5_C_INCLUDE_DIRS})

add_executable(testio test/shapefile_test.cpp test/csv_test.cpp test/hdf_test.cpp)
target_link_libraries(testio Catch2::Catch2 io Boost::filesystem)
add_test(testio testio WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/io/test)
