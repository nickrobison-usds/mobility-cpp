
find_package(GEOS REQUIRED)

add_library(components WeekSplitter.cpp include/components/WeekSplitter.hpp include/components/data.hpp server/LocationJoinerServer.cpp server/LocationJoinerServer.hpp LocationJoiner.cpp include/components/LocationJoiner.hpp server/WeekSplitterServer.cpp server/WeekSplitterServer.hpp)
target_link_libraries(components PUBLIC hpx PRIVATE io spdlog GEOS::GEOS)
target_include_directories(components PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)