//
// Created by Nicholas Robison on 7/8/20.
//

#include <mpi.h>
#include "ParallelHDF5Table.hpp"

namespace io {

    ParallelHDF5Table::ParallelHDF5Table() {
        int mpi_size, mpi_rank;
        MPI_Comm comm = MPI_COMM_WORLD;
        MPI_Info info = MPI_INFO_NULL;
        MPI_Init(nullptr, nullptr);
        MPI_Comm_size(comm, &mpi_size);
        MPI_Comm_rank(comm, &mpi_rank);
    }

    ParallelHDF5Table::~ParallelHDF5Table() {
        MPI_Finalize();
    }
}