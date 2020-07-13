//
// Created by Nicholas Robison on 7/8/20.
//

#ifndef MOBILITY_CPP_PARALLELHDF5_HPP
#define MOBILITY_CPP_PARALLELHDF5_HPP

#include <mpi.h>
#include <hdf5.h>
#include "string"
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

namespace io {

    template<class DataType, int Dimensions>
    class ParallelHDF5 {
    public:
        ParallelHDF5(const std::string &filename, const std::string &dsetname, std::array<hsize_t, Dimensions> &dims)
                : _dimensions(dims) {
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

            // Register the type
            constexpr std::size_t type_sz = sizeof(DataType);
            constexpr int columns = DataType::columns;
            constexpr std::array<const char *, columns> vals = DataType::names();
            constexpr std::array<size_t, columns> offsets = DataType::offsets();
            // This can't be constexpr, because the HDF5 methods aren't constexpr
            std::array<const hid_t, columns> types = DataType::types();
            _data_type = H5Tcreate(H5T_COMPOUND, type_sz);

            herr_t status;
            for (std::size_t i = 0 ; i < columns; i ++) {
                status = H5Tinsert(_data_type, vals[i], offsets[i], types[i]);
                if (status) {
                    std::cout << status << std::endl;
                }
            }

            // Create the dataset
            const auto filespace = H5Screate_simple(Dimensions, _dimensions.data(), nullptr);

            _dset_id = H5Dcreate2(_file_id, dsetname.c_str(), _data_type, filespace, H5P_DEFAULT, H5P_DEFAULT,
                                  H5P_DEFAULT);
            H5Sclose(filespace);
        }

        void write(const std::array<hsize_t, Dimensions> &count, const std::array<hsize_t, Dimensions> offset, const std::vector<DataType> &data) {

            _memspace = H5Screate_simple(Dimensions, count.data(), nullptr);
            _filespace = H5Dget_space(_dset_id);
            H5Sselect_hyperslab(_filespace, H5S_SELECT_SET, offset.data(), nullptr, count.data(), nullptr);

            const auto plist_id = H5Pcreate(H5P_DATASET_XFER);
            H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);

            const herr_t status = H5Dwrite(_dset_id, _data_type, _memspace, _filespace, plist_id, data.data());

            if (status) {
                std::cout << status << std::endl;
            }
        }

        std::vector<DataType> read(const std::array<hsize_t, Dimensions> &count, const std::array<hsize_t, Dimensions> offset) const {

            // Create a memory space to read into
            const auto memspace = H5Screate_simple(Dimensions, count.data(), nullptr);

            const auto dataspace = H5Dget_space(_dset_id);
            auto status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset.data(), nullptr, count.data(),
                                                      nullptr);
            if (status) {
                std::cout << status << std::endl;
            }

            // Compute return size;
            const std::size_t rs = std::accumulate(count.begin(), count.end(), 1,std::multiplies<size_t>());
            std::vector<DataType> results(rs);

            status = H5Dread(_dset_id, _data_type, memspace, dataspace, H5P_DEFAULT, results.data());
            if (status) {
                std::cout << status << std::endl;
            }
            // TODO: This needs to be closed correctly.
            H5Sclose(memspace);
            H5Sclose(dataspace);

            return results;
        }

        [[nodiscard]] std::string hello() const {
            return "hello";
        }

        ~ParallelHDF5() {
            H5Tclose(_data_type);
            H5Sclose(_filespace);
            H5Sclose(_memspace);
            H5Dclose(_dset_id);
            H5Fclose(_file_id);
        }

    private:
        int mpi_size;
        int mpi_rank;
        hid_t _file_id;
        hid_t _dset_id;
        hid_t _filespace;
        hid_t _memspace;
        hid_t _data_type;
        const std::array<hsize_t, Dimensions> _dimensions;
    };
}


#endif //MOBILITY_CPP_PARALLELHDF5_HPP
