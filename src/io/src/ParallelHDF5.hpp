//
// Created by Nicholas Robison on 7/8/20.
//

#ifndef MOBILITY_CPP_PARALLELHDF5_HPP
#define MOBILITY_CPP_PARALLELHDF5_HPP

#include <mpi.h>
#include <hdf5.h>
#include "string"
#include <iostream>

namespace io {
    template<class DataType, int Dimensions>
    class ParallelHDF5 {
    public:
        ParallelHDF5(const std::string &filename, const std::string &dsetname, std::array<hsize_t, Dimensions> &dims): _dimensions(dims) {
            // Initialize the MPI values
            MPI_Comm comm = MPI_COMM_WORLD;
            MPI_Info info = MPI_INFO_NULL;
            MPI_Comm_size(comm, &mpi_size);
            MPI_Comm_rank(comm, &mpi_rank);

            // Create the file
            const auto plist_id = H5Pcreate(H5P_FILE_ACCESS);
            H5Pset_fapl_mpio(plist_id, comm, info);
            _file_id = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);
            H5Pclose(plist_id);

            // Create the dataset
            const auto filespace = H5Screate_simple(Dimensions, _dimensions.data(), nullptr);

            _dset_id = H5Dcreate2(_file_id, dsetname.c_str(), H5T_NATIVE_INT, filespace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            H5Sclose(filespace);
        }

        void write(const std::size_t offset, const std::size_t count, const std::vector<DataType> &data) {

            // Create the required dimensions
            std::array<hsize_t, Dimensions> count_arry;
            std::array<hsize_t, Dimensions> offset_arry;
            for(int i = 0; i < Dimensions; i ++) {
                count_arry[i] = _dimensions[i];
                offset_arry[i] = 0;
            }
            // Set the count based on the tile size along with the starting offset
            count_arry[0] = count;
            offset_arry[0] = offset;


            _memspace = H5Screate_simple(Dimensions, count_arry.data(), nullptr);
            _filespace = H5Dget_space(_dset_id);
            H5Sselect_hyperslab(_filespace, H5S_SELECT_SET, offset_arry.data(), nullptr, count_arry.data(), nullptr);

            const auto plist_id = H5Pcreate(H5P_DATASET_XFER);
            H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);

            const herr_t status = H5Dwrite(_dset_id, H5T_NATIVE_INT, _memspace, _filespace, plist_id, data.data());

            if (status) {
                std::cout << status << std::endl;
            }
        }




        const std::string hello() const {
            return "hello";
        }

    private:
        int mpi_size;
        int mpi_rank;
        hid_t _file_id;
        hid_t _dset_id;
        hid_t _filespace;
        hid_t _memspace;
        const std::array<hsize_t, Dimensions> _dimensions;
    };
}


#endif //MOBILITY_CPP_PARALLELHDF5_HPP
