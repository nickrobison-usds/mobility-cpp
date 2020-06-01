
find_package(GEOS REQUIRED)

#Set options for Blaze Library
set(BLAZE_SMP_THREADS HPX)

add_library(components WeekSplitter.cpp include/components/WeekSplitter.hpp include/components/data.hpp server/JoinedLocationServer.cpp server/JoinedLocationServer.hpp JoinedLocation.cpp include/components/JoinedLocation.hpp server/WeekSplitterServer.cpp server/WeekSplitterServer.hpp server/TileServer.cpp server/TileServer.hpp TileDimension.cpp TileDimension.hpp)
target_link_libraries(components PUBLIC hpx PRIVATE io spdlog GEOS::GEOS blaze)
target_include_directories(components PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)