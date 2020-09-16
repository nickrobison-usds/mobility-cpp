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
        src/server/CBGShapefileServer.cpp include/components/server/CBGShapefileServer.hpp
        src/CBGShapefileWrapper.cpp include/components/CBGShapefileWrapper.hpp
        src/TemporalMatricies.cpp include/components/TemporalMatricies.hpp
        src/TileWriter.cpp include/components/TileWriter.hpp src/RowProcessor.cpp include/components/RowProcessor.hpp src/detail/CBGOffsetCalculator.cpp include/components/detail/CBGOffsetCalculator.hpp
        src/VisitMatrixWriter.cpp include/components/VisitMatrixWriter.hpp TemporalGraphs.cpp include/components/TemporalGraphs.hpp include/components/BaseSafegraphMapper.hpp src/BaseSafegraphMapper.cpp include/components/OffsetCalculator.hpp src/detail/CountyOffsetCalculator.cpp include/components/detail/CountyOffsetCalculator.hpp include/components/detail/offset_shared.hpp src/server/CountyShapefileServer.cpp include/components/server/CountyShapefileServer.hpp src/CountyShapefileWrapper.cpp include/components/CountyShapefileWrapper.hpp src/detail/helpers.cpp include/components/detail/helpers.hpp)

add_library(MCPP::components ALIAS mcpp-components)

target_link_libraries(mcpp-components PUBLIC HPX::hpx blaze date absl::flat_hash_map io MCPP::graph MCPP::map-tile PRIVATE spdlog absl::synchronization Boost::regex shared)
target_include_directories(mcpp-components PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)
target_compile_options(mcpp-components PUBLIC "-march=native")

add_sanitizers(mcpp-components)


#add_executable(testcomponents test/test_scaler.cpp)
#target_link_libraries(testcomponents Catch2::Catch2 MCPP::components io)
#add_test(testcomponents testcomponents)