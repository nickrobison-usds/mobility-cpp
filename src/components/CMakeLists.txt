
find_package(GEOS REQUIRED)

# Include AVX, if it's available
include(FindAVX)
CHECK_FOR_AVX()

add_library(mcpp-components
        src/WeekSplitter.cpp
# src/vector_scaler.hpp
        include/components/WeekSplitter.hpp
        src/server/JoinedLocationServer.cpp src/server/JoinedLocationServer.hpp
        src/JoinedLocation.cpp include/components/JoinedLocation.hpp
        src/server/WeekSplitterServer.cpp src/server/WeekSplitterServer.hpp
        src/server/TileServer.cpp src/server/TileServer.hpp
        include/components/TileClient.hpp src/TileClient.cpp
        src/server/ShapefileServer.cpp src/server/ShapefileServer.hpp
        src/ShapefileWrapper.cpp include/components/ShapefileWrapper.hpp
        src/TemporalMatricies.cpp src/TemporalMatricies.hpp
        src/TileWriter.cpp src/TileWriter.hpp src/RowProcessor.cpp include/components/RowProcessor.hpp src/OffsetCalculator.cpp include/components/OffsetCalculator.hpp
        src/VisitMatrixWriter.cpp src/VisitMatrixWriter.hpp)

add_library(MCPP::components ALIAS mcpp-components)

target_link_libraries(mcpp-components PUBLIC HPX::hpx blaze date absl::flat_hash_map io PRIVATE spdlog GEOS::GEOS absl::synchronization Boost::regex shared)
target_include_directories(mcpp-components PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_compile_options(mcpp-components PUBLIC "-march=native")

add_sanitizers(mcpp-components)


#add_executable(testcomponents test/test_scaler.cpp)
#target_link_libraries(testcomponents Catch2::Catch2 MCPP::components io)
#add_test(testcomponents testcomponents)