add_library(map-tile src/server/MapTileServer.cpp include/map-tile/MapTileServer.hpp src/FileProvider.cpp src/FileProvider.hpp)

target_link_libraries(map-tile PUBLIC HPX::hpx PRIVATE Boost::iostreams io spdlog shared)
target_include_directories(map-tile PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_executable(testmap-tile test/simple_test.cpp)
target_link_libraries(testmap-tile Catch2::Catch2 map-tile io Boost::filesystem)
add_test(testmap-tile testmap-tile WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/map-tile/test)