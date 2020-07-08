//
// Created by Nicholas Robison on 7/8/20.
//

#ifndef MOBILITY_CPP_PARALLELHDF5TABLE_HPP
#define MOBILITY_CPP_PARALLELHDF5TABLE_HPP

namespace io {
    class ParallelHDF5Table {
        explicit ParallelHDF5Table();
        ParallelHDF5Table(const ParallelHDF5Table &mpi) = delete;
        ParallelHDF5Table &operator=(const ParallelHDF5Table &mpi) = delete;
        ~ParallelHDF5Table();
    };
}


#endif //MOBILITY_CPP_PARALLELHDF5TABLE_HPP
