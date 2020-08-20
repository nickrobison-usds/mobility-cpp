add_library(graph include/graph/graph.hpp src/BoostGraph.cpp include/graph/BoostGraph.hpp)
add_library(MCPP::graph ALIAS graph)
target_link_libraries(graph PUBLIC absl::flat_hash_map PRIVATE spdlog)

target_include_directories(graph PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_library(MCPP::Graph ALIAS graph)

add_executable(graph-tests test/distance-test.cpp)
target_link_libraries(graph-tests PRIVATE Catch2::Catch2 MCPP::Graph absl::strings)
add_test(graph graph-tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/graph/test)
