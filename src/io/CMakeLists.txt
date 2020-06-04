find_package(GDAL REQUIRED)

add_library(io src/parquet.cpp include/io/parquet.hpp src/Shapefile.cpp include/io/Shapefile.hpp include/io/csv_reader.hpp include/io/csv.hpp)
target_link_libraries(io PUBLIC arrow parquet GDAL PRIVATE spdlog Boost::filesystem)
target_include_directories(io PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_executable(testio test/shapefile_test.cpp test/csv_test.cpp)
target_link_libraries(testio Catch2::Catch2 io)
add_test(testio testio WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/io/test)