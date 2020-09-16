# Find HDF5
set(HDF5_PREFER_PARALLEL False)

add_library(io src/parquet.cpp include/io/parquet.hpp src/Shapefile.cpp include/io/Shapefile.hpp include/io/csv_reader.hpp include/io/csv.hpp include/io/hdf5.hpp)
target_link_libraries(io PUBLIC GDAL::GDAL arrow parquet ${HDF5_C_LIBRARIES} ${HDF5_C_HL_LIBRARIES} PRIVATE spdlog)
target_include_directories(io PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${HDF5_C_INCLUDE_DIRS})

# Temporarily disable the HDF5 test, as there are some linking issues.
# I think we're going to remove it anyways.
add_executable(testio test/shapefile_test.cpp test/csv_test.cpp)
target_link_libraries(testio Catch2::Catch2 io Boost::filesystem)
add_test(testio testio WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/io/test)

add_executable(io-benchmarks benchmarks/simple.cpp benchmarks/row_expansion.cpp)
target_link_libraries(io-benchmarks arrow benchmark::benchmark_main)
target_compile_options(io-benchmarks PRIVATE ${RELEASE_FLAGS})

# If we're building arrow locally, make sure we build it first
if (${USE_INTERNAL_ARROW})
    add_dependencies(io arrow_ep)
    add_dependencies(io-benchmarks arrow_ep)
    # Gross, but for some reason, we need this to make sure the parquet lib is found correctly
    target_link_directories(io PUBLIC ${CMAKE_BINARY_DIR}/arrow/lib)
    target_include_directories(io PUBLIC ${CMAKE_BINARY_DIR}/arrow/include)
    target_link_directories(io-benchmarks PUBLIC ${CMAKE_BINARY_DIR}/arrow/lib)
    target_include_directories(io-benchmarks PUBLIC ${CMAKE_BINARY_DIR}/arrow/include)
endif ()

if (${USE_INTERNAL_HDF5})
    add_dependencies(io hdf5_ep)
endif()