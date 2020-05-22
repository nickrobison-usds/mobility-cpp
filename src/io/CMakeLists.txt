find_package(GDAL REQUIRED)

add_library(io src/parquet.cpp include/io/parquet.hpp src/shapefile.cpp include/io/shapefile.hpp)
target_link_libraries(io PUBLIC arrow parquet GDAL PRIVATE spdlog)
target_include_directories(io PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_executable(testio test/shapefile_test.cpp)
target_link_libraries(testio Catch2::Catch2 io)
add_test(testio testio)