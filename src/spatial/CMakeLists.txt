find_package(GDAL REQUIRED)

add_library(spatial SHARED
        include/spatial/SpatialProcessor.hpp src/Algorithms.cpp
        src/GEOSProcessor.cpp
        include/spatial/GEOSProcessor.hpp)

target_link_libraries(spatial PUBLIC ${GDAL_LIBRARY} PRIVATE)
target_include_directories(spatial PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

# Apple metal
if (${USE_METAL})
    target_link_libraries(spatial PRIVATE "-framework Metal -framework Foundation")
    target_sources(spatial PRIVATE src/MetalProcessor.mm include/spatial/MetalProcessor.hpp src/haversine.metal ${CMAKE_CURRENT_BINARY_DIR}/spatial.metallib)

    add_custom_command(OUTPUT haversine.air
            COMMAND xcrun ARGS -sdk macosx metal -c ${CMAKE_CURRENT_SOURCE_DIR}/src/haversine.metal -o haversine.air
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Compiling haversine")

    set(RESOURCE_FILES src/haversine.metal ${CMAKE_CURRENT_BINARY_DIR}/spatial.metallib)

    add_custom_command(OUTPUT spatial.metallib
            COMMAND xcrun ARGS -sdk macosx metallib haversine.air -o spatial.metallib
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Compiling spatial metallib"
            DEPENDS haversine.air)

    set_target_properties(spatial PROPERTIES
            MACOSX_FRAMEWORK_IDENTIFIER com.nickrobison.spatial
            FRAMEWORK TRUE
            RESOURCE "${RESOURCE_FILES}")

    add_custom_target(spatial-metal DEPENDS spatial.metallib)
    add_dependencies(spatial spatial-metal)
endif ()

add_executable(spatial-tests test/simple-tests.cpp)
target_link_libraries(spatial-tests Catch2::Catch2 spatial)
add_test(spatial-tests spatial-tests)