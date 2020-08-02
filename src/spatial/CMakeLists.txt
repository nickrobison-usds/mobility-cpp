find_package(GDAL REQUIRED)

add_library(spatial
        include/spatial/SpatialProcessor.hpp src/Algorithms.cpp
        src/GEOSProcessor.cpp
        include/spatial/GEOSProcessor.hpp
        src/MetalProcessor.mm include/spatial/MetalProcessor.hpp src/haversine.metal)

target_link_libraries(spatial PUBLIC GDAL::GDAL)
target_include_directories(spatial PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_executable(spatial-tests test/simple-tests.cpp)
target_link_libraries(spatial-tests Catch2::Catch2 spatial)
add_test(spatial-tests spatial-tests)