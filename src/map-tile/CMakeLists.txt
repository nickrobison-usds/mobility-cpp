# We don't require Eigen to build the project, but the tests do require it
find_package(Eigen3 3.3 NO_MODULE)

set(EMIT_SIZE 4096 CACHE STRING "Buffer size (bytes) before emitting")


add_library(map-tile include/map-tile/server/MapTileServer.hpp src/FileProvider.cpp include/map-tile/io/FileProvider.hpp include/map-tile/ctx/Context.hpp include/map-tile/client/MapTileClient.hpp include/map-tile/coordinates/LocaleLocator.hpp src/coordinates/Coordinate2D.cpp include/map-tile/coordinates/Coordinate2D.hpp src/coordinates/LocaleTiler.cpp include/map-tile/coordinates/LocaleTiler.hpp src/coordinates/Coordinate3D.cpp include/map-tile/coordinates/Coordinate3D.hpp include/traits.hpp include/map-tile/io/EmitHandler.hpp src/io/EmitHelpers.cpp include/map-tile/io/EmitHelpers.hpp include/map-tile/MapTileBuilder.hpp include/map-tile/MapTile.hpp)

target_link_libraries(map-tile PUBLIC HPX::hpx spdlog shared Boost::filesystem range-v3 PRIVATE Boost::iostreams)
target_include_directories(map-tile PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_compile_definitions(map-tile PUBLIC EMIT_SIZE=${EMIT_SIZE})

add_library(MCPP::map-tile ALIAS map-tile)

add_executable(testmap-tile-runtime test/runtime_tests.cpp)
target_link_libraries(testmap-tile-runtime HPX::wrap_main Catch2::Catch2 MCPP::map-tile io Boost::filesystem absl::strings Eigen3::Eigen)
add_test(map-tile testmap-tile-runtime WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/map-tile/test)

add_executable(map-tile-unit-tests test/locator_tests.cpp test/serialization_tests.cpp test/tiler_tests.cpp)
target_link_libraries(map-tile-unit-tests Catch2::Catch2 MCPP::map-tile io Boost::filesystem absl::strings)
add_test(map-tile map-tile-unit-tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/map-tile/test)