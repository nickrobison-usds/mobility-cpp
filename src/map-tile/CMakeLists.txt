# We don't require Eigen to build the project, but the tests do require it
find_package (Eigen3 3.3 NO_MODULE)


add_library(map-tile include/map-tile/server/MapTileServer.hpp src/FileProvider.cpp src/FileProvider.hpp include/map-tile/ctx/Context.hpp include/map-tile/client/MapTileClient.hpp src/coordinates/LocaleLocator.cpp include/map-tile/coordinates/LocaleLocator.hpp src/coordinates/Coordinate2D.cpp include/map-tile/coordinates/Coordinate2D.hpp src/coordinates/LocaleTiler.cpp src/coordinates/LocaleTiler.hpp)

target_link_libraries(map-tile PUBLIC HPX::hpx INTERFACE spdlog PRIVATE Boost::iostreams io shared)
target_include_directories(map-tile PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_executable(testmap-tile-runtime test/runtime_tests.cpp)
target_link_libraries(testmap-tile-runtime Catch2::Catch2 map-tile io Boost::filesystem absl::strings Eigen3::Eigen)
add_test(testmap-tile-runtime testmap-tile-runtime WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/map-tile/test)

add_executable(map-tile-unit-tests test/locator_tests.cpp test/serialization_tests.cpp test/tiler_tests.cpp)
target_link_libraries(map-tile-unit-tests Catch2::Catch2 map-tile io Boost::filesystem absl::strings)
add_test(map-tile-unit-tests map-tile-unit-tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/map-tile/test)