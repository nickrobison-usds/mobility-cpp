
find_package(GEOS REQUIRED)

#Set options for Blaze Library
set(BLAZE_SMP_THREADS HPX)

add_library(components
        WeekSplitter.cpp
        include/components/WeekSplitter.hpp include/components/data.hpp
        include/components/serializers.hpp include/components/serializers.cpp
        server/JoinedLocationServer.cpp server/JoinedLocationServer.hpp
        JoinedLocation.cpp include/components/JoinedLocation.hpp
        server/WeekSplitterServer.cpp server/WeekSplitterServer.hpp
        server/TileServer.cpp server/TileServer.hpp
        TileDimension.hpp include/components/TileClient.hpp TileClient.cpp
        server/ShapefileServer.cpp server/ShapefileServer.hpp
        ShapefileWrapper.cpp include/components/ShapefileWrapper.hpp
        TemporalMatricies.cpp TemporalMatricies.hpp)
target_link_libraries(components PUBLIC hpx blaze date PRIVATE io spdlog GEOS::GEOS absl::flat_hash_map absl::strings absl::synchronization Boost::regex)
target_include_directories(components PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)