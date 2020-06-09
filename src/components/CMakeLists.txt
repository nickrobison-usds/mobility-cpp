
find_package(GEOS REQUIRED)

# Include AVX, if it's available
include(FindAVX)
CHECK_FOR_AVX()

add_library(components
        src/WeekSplitter.cpp src/vector_scaler.hpp include/components/constants.hpp
        include/components/WeekSplitter.hpp include/components/data.hpp
        include/components/serializers.hpp include/components/serializers.cpp
        src/server/JoinedLocationServer.cpp src/server/JoinedLocationServer.hpp
        src/JoinedLocation.cpp include/components/JoinedLocation.hpp
        src/server/WeekSplitterServer.cpp src/server/WeekSplitterServer.hpp
        src/server/TileServer.cpp src/server/TileServer.hpp
        src/TileConfiguration.hpp include/components/TileClient.hpp src/TileClient.cpp
        src/server/ShapefileServer.cpp src/server/ShapefileServer.hpp
        src/ShapefileWrapper.cpp include/components/ShapefileWrapper.hpp
        src/TemporalMatricies.cpp src/TemporalMatricies.hpp
        src/TileWriter.cpp src/TileWriter.hpp src/RowProcessor.cpp src/RowProcessor.hpp src/types.hpp)
target_link_libraries(components PUBLIC hpx blaze date PRIVATE io spdlog GEOS::GEOS absl::flat_hash_map absl::strings absl::synchronization Boost::regex)
target_include_directories(components PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_compile_options(components PUBLIC "-march=native")


add_executable(testcomponents test/test_scaler.cpp)
target_link_libraries(testcomponents Catch2::Catch2 components)
add_test(testcomponents testcomponents)