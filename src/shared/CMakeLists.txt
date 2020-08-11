add_library(shared
        src/HostnameLogger.cpp include/shared/HostnameLogger.hpp
        src/DateUtils.cpp include/shared/DateUtils.hpp
        src/DirectoryUtils.cpp include/shared/DirectoryUtils.hpp
        include/shared/serializers.hpp src/serializers.cpp
        include/shared/data.hpp
        include/shared/TileConfiguration.hpp include/shared/constants.hpp src/ConversionUtils.cpp include/shared/ConversionUtils.hpp include/shared/QuotedLineSplitter.hpp)

target_link_libraries(shared PUBLIC spdlog date HPX::hpx arrow PRIVATE Boost::system)
target_include_directories(shared PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_executable(shared-tests test/serialization_tests.cpp)
target_link_libraries(shared-tests shared Catch2::Catch2)
add_test(shared-tests shared-tests)

# If we're building arrow locally, make sure we build it first
if (${USE_INTERNAL_ARROW})
    add_dependencies(shared arrow_ep)
    # Gross, but for some reason, we need this to make sure the parquet lib is found correctly
    target_link_directories(shared PUBLIC ${CMAKE_BINARY_DIR}/arrow/lib)
    target_include_directories(shared PUBLIC ${CMAKE_BINARY_DIR}/arrow/include)
endif ()