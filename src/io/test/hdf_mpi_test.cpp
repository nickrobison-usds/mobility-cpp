//
// Created by Nicholas Robison on 7/9/20.
//

#define CATCH_CONFIG_RUNNER

#include <mpi.h>
#include <array>
#include "catch2/catch.hpp"
#include "../src/ParallelHDF5.hpp"

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int result = Catch::Session().run(argc, argv);
    MPI_Finalize();
    return result;
}

TEST_CASE("2D dataset set", "[hdf5-par]") {
    const std::string filename = "./test-par.h5";
    const std::string datasetname = "par-dset";
    const std::vector<int> data{1, 2, 3, 4, 5, 6};
    std::array<hsize_t , 2> dims{2, 3};
    int mpi_size, mpi_rank;
    MPI_Comm comm  = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &mpi_size);
    MPI_Comm_rank(comm, &mpi_rank);
    io::ParallelHDF5<int, 2> p5("./test-par.h5", "test-dataset", dims);
    {
        p5.write(dims, {0, 0}, data);
    }

    // Read everything back out
    const auto results = p5.read({2, 3}, {0, 0});
    REQUIRE(results == data);

    // Read only a portion of everything
    const auto slice = p5.read({2, 2}, {0, 0});
    REQUIRE(slice.size() == 4);
    REQUIRE(std::accumulate(slice.begin(), slice.end(), 0.0) == 12.0);

    // Get the last two elements of reach row
    const auto row_slice = p5.read({2, 1}, {0, 2});
    REQUIRE(row_slice.size() == 2);
    REQUIRE(row_slice.at(0) == 3);
    REQUIRE(row_slice.at(1) == 6);
}

TEST_CASE("3D dataset set", "[hdf5-par]") {
    const std::string filename = "./test-par.h5";
    const std::string datasetname = "par-dset";
    const std::vector<int> data{1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14, 15};
    std::array<hsize_t , 3> dims{2, 3,2};
    io::ParallelHDF5<int, 3> p5("./test-par.h5", "test-dataset", dims);
    {
        p5.write(dims, {0, 0, 0}, data);
    }

    // Read everything back out
    const auto results = p5.read({2, 3, 2}, {0, 0, 0});
    REQUIRE(results == data);

    // Read the first row
    const auto row_results = p5.read({1, 3, 1}, {1, 0, 0});
    REQUIRE(row_results.size() == 3);
    REQUIRE(row_results.at(0) == 10);
    REQUIRE(row_results.at(2) == 14);

    // Read the second Z tile
    const auto second_tile = p5.read({2, 3, 1}, {0, 0, 1});
    REQUIRE(second_tile.size() == 6);
    REQUIRE(std::accumulate(second_tile.begin(), second_tile.end(), 0) == 51);

    // Read a single Z-index
    const auto middle_elems = p5.read({1, 1, 2}, {1, 1, 0});
    REQUIRE(middle_elems.size() == 2);
    REQUIRE(std::accumulate(middle_elems.begin(), middle_elems.end(), 0) == 25);
}