//
// Created by Nicholas Robison on 5/28/20.
//

#ifndef MOBILITY_CPP_HDF5_HPP
#define MOBILITY_CPP_HDF5_HPP

#include "hdf5.h"
#include <string>
#include <array>
#include <iostream>
#include <utility>
#include "hdf5_hl.h"

namespace io {

    hid_t openFile(const std::string &filename) {
        return H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    }


    template<class Row>
    class HDF5Table {

    public:
        explicit HDF5Table(const std::string &filename, std::string tablename, std::string tabletitle) : _hdf5(
                openFile(filename)), _table_name(std::move(tablename)), _table_title(std::move(tabletitle)) {};

        HDF5Table(const HDF5Table &hdf5) = delete;

        HDF5Table &operator=(const HDF5Table &hdf5) = delete;

        ~HDF5Table() {
            H5Fclose(_hdf5);
        };

        void createTable() const {
            constexpr size_t recordSize = sizeof(Row);
            constexpr int columns = Row::columns;
            constexpr std::array<const char *, columns> vals = Row::names();
            constexpr std::array<size_t, columns> offsets = Row::offsets();
            // This can't be constexpr, because the HDF5 methods aren't constexpr
            std::array<const hid_t, columns> types = Row::types();

            const auto err = H5TBmake_table(_table_title.c_str(), _hdf5, _table_name.c_str(), vals.size(), 0,
                                            recordSize, const_cast<const char **>(vals.data()), offsets.data(),
                                            types.data(),
                                            100, nullptr, 0, nullptr);

            if (err) {
                std::cout << err << std::endl;
            }
        };

        void writeRows(std::vector<Row> rows) const {
            constexpr size_t recordSize = sizeof(Row);
            constexpr int columns = Row::columns;
            constexpr std::array<const char *, columns> vals = Row::names();
            constexpr std::array<size_t, columns> offsets = Row::offsets();
            constexpr std::array<size_t, columns> field_sizes = Row::field_sizes();
            const size_t num_rows = rows.size();

            const auto err = H5TBappend_records(_hdf5, _table_name.c_str(), num_rows, recordSize, offsets.data(),
                                                field_sizes.data(), rows.data());
            if (err) {
                std::cout << err << std::endl;
            }
        }

        std::vector<Row> readRows() const {
            constexpr size_t recordSize = sizeof(Row);
            constexpr int columns = Row::columns;
            constexpr std::array<const char *, columns> vals = Row::names();
            constexpr std::array<size_t, columns> offsets = Row::offsets();
            constexpr std::array<size_t, columns> field_sizes = Row::field_sizes();

            // Figure out how many rows to read
            hsize_t field_count, row_count;
            auto err = H5TBget_table_info(_hdf5, _table_name.c_str(), &field_count, &row_count);

            std::vector<Row> rows(row_count);
            err = H5TBread_table(_hdf5, _table_name.c_str(), recordSize, offsets.data(), field_sizes.data(), rows.data());

            if (err) {
                std::cout << err << std::endl;
            }

            return rows;
        }


    private:
        const hid_t _hdf5;
        const std::string _table_name;
        const std::string _table_title;
    };
}


#endif //MOBILITY_CPP_HDF5_HPP
