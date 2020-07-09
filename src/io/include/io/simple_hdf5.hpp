//
// Created by Nicholas Robison on 5/28/20.
//

#ifndef MOBILITY_CPP_SIMPLE_HDF5_HPP
#define MOBILITY_CPP_SIMPLE_HDF5_HPP

#include "hdf5.h"
#include <string>
#include <array>
#include <iostream>
#include <utility>
#include "hdf5_hl.h"

namespace io {

    hid_t openSerialFile(const std::string &filename) {
        return H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    }

    class SerialHDF5Provider {
    public:
        SerialHDF5Provider(const std::string &filename): _file_id(openSerialFile(filename)) {

        }

        SerialHDF5Provider(const SerialHDF5Provider &provider) = delete;
        SerialHDF5Provider &operator=(const SerialHDF5Provider &provider) = delete;
        ~SerialHDF5Provider() {
            H5Fclose(_file_id);
        }


        const hid_t& provide() const {
            return _file_id;
        }

    private:
        const hid_t _file_id;
    };


    template<class Row, class HDF5Provider = SerialHDF5Provider>
    class SimpleHDF5Table {

    public:
        explicit SimpleHDF5Table(const std::string &filename, std::string tablename, std::string tabletitle) : _provider(
                HDF5Provider(filename)), _table_name(std::move(tablename)), _table_title(std::move(tabletitle)) {};

        void createTable() const {
            constexpr size_t recordSize = sizeof(Row);
            constexpr int columns = Row::columns;
            constexpr std::array<const char *, columns> vals = Row::names();
            constexpr std::array<size_t, columns> offsets = Row::offsets();
            // This can't be constexpr, because the HDF5 methods aren't constexpr
            std::array<const hid_t, columns> types = Row::types();
            const auto hdf5_table = _provider.provide();

            const auto err = H5TBmake_table(_table_title.c_str(), hdf5_table, _table_name.c_str(), vals.size(), 0,
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
            const auto hdf5_table = _provider.provide();

            const auto err = H5TBappend_records(hdf5_table, _table_name.c_str(), num_rows, recordSize, offsets.data(),
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
            const auto hdf5_table = _provider.provide();

            // Figure out how many rows to read
            hsize_t field_count, row_count;
            auto err = H5TBget_table_info(hdf5_table, _table_name.c_str(), &field_count, &row_count);
            if (err) {
                std::cout << err << std::endl;
            }

            std::vector<Row> rows(row_count);
            err = H5TBread_table(hdf5_table, _table_name.c_str(), recordSize, offsets.data(), field_sizes.data(), rows.data());

            if (err) {
                std::cout << err << std::endl;
            }

            return rows;
        }


    private:
        const HDF5Provider _provider;
        const std::string _table_name;
        const std::string _table_title;
    };
}


#endif //MOBILITY_CPP_SIMPLE_HDF5_HPP
