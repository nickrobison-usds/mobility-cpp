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

TEST_CASE("MPI Initializes", "[hdf5-par]") {
    const std::string filename = "./test-par.h5";
    const std::string datasetname = "par-dset";
    const std::vector<int> data{1, 2, 3, 4, 5, 6};
    std::array<hsize_t , 2> dims{2, 3};
    {
        int mpi_size, mpi_rank;
        MPI_Comm comm  = MPI_COMM_WORLD;
        MPI_Comm_size(comm, &mpi_size);
        MPI_Comm_rank(comm, &mpi_rank);
        io::ParallelHDF5<int, 2> p5("./test-par.h5", "test-dataset", dims);
        const auto h = p5.hello();
        REQUIRE(h == "hello");
        p5.write(mpi_rank * 2, 2 / mpi_size, data);
    }

    // Now, read it back out


}