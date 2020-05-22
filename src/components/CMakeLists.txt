
find_package(GEOS REQUIRED)

add_library(components WeekSplitter.cpp WeekSplitter.hpp data.hpp)
target_link_libraries(components PUBLIC hpx PRIVATE io spdlog GEOS::GEOS)