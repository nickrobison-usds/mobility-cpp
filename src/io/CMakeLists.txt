find_package(GDAL REQUIRED)

# Find HDF5
set(HDF5_PREFER_PARALLEL True)

find_package(HDF5 REQUIRED COMPONENTS HL)

if (NOT HDF5_IS_PARALLEL)
    message(FATAL_ERROR "HDF5 C++ library must be thread-safe")
endif ()

add_library(io src/parquet.cpp include/io/parquet.hpp src/Shapefile.cpp include/io/Shapefile.hpp include/io/csv_reader.hpp include/io/csv.hpp include/io/simple_hdf5.hpp include/io/hdf5.hpp include/io/helpers.hpp)
target_link_libraries(io PUBLIC arrow parquet gdal hdf5_hl-static hdf5-static MPI::MPI_CXX PRIVATE spdlog)
target_include_directories(io PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

add_executable(testio test/shapefile_test.cpp test/csv_test.cpp test/hdf_test.cpp)
target_link_libraries(testio Catch2::Catch2 io Boost::filesystem)
add_test(testio testio WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/io/test)

add_executable(testio_mpi test/hdf_mpi_test.cpp)
target_link_libraries(testio_mpi Catch2::Catch2 io Boost::filesystem spdlog)
set(test_parameters ${MPIEXEC_NUMPROC_FLAG} 2 "./testio_mpi")
set(MPIEXEC_NUMPROC_FLAG 5)
add_test(testio_mpi testio_mpi WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/io/test COMMAND ${MPIEXEC} ${test_parameters})
message(STATUS "Run: ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} ${MPIEXEC_MAX_NUMPROCS} ${MPIEXEC_PREFLAGS} EXECUTABLE ${MPIEXEC_POSTFLAGS} ARGS")