//
// Created by Nicholas Robison on 5/28/20.
//

#include <io/sizer.hpp>
#include <ostream>
#include "catch2/catch.hpp"
#include "io/hdf5.hpp"
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

inline int64_t stringSize(int maxSize) {
    auto string_type = H5Tcopy(H5T_C_S1);
    const auto err = H5Tset_size(string_type, maxSize);
    if (err) {
        std::cout << err << std::endl;
    }
    return string_type;
}

inline bool compare_float(double x, double y, double epsilon = 0.0000001f){
    return fabs(x - y) < epsilon;
}

struct sample_row {
    char name[16];
    int value;
    double d_value;

    bool operator==(const sample_row &rhs) const {
        const auto c = strcmp(name, rhs.name) == 0;
        const auto d = compare_float(d_value, rhs.d_value);
        return c &&
               value == rhs.value &&
               d;
    }

    bool operator!=(const sample_row &rhs) const {
        return !(rhs == *this);
    }

    friend std::ostream &operator<<(std::ostream &os, const sample_row &row) {
        os << "name: " << row.name << " value: " << row.value << " d_value: " << row.d_value;
        return os;
    }

    static constexpr int columns = 3;


    static constexpr std::array<const char *, sample_row::columns> names() {
        return {"Name", "Value", "DValue"};
    }

    static std::array<const int64_t, sample_row::columns> types() {
        return {
                stringSize(16),
                H5T_NATIVE_INT,
                H5T_NATIVE_DOUBLE
        };
    }

    static constexpr std::array<size_t, sample_row::columns> offsets() {
        return {
                offset_of<sample_row>(&sample_row::name),
                offset_of<sample_row>(&sample_row::value),
                offset_of<sample_row>(&sample_row::d_value),
        };
    }

    static constexpr std::array<size_t, sample_row::columns> field_sizes() {
        return {
                sizeof(sample_row::name),
                sizeof(sample_row::value),
                sizeof(sample_row::d_value)
        };
    }
};


TEST_CASE("Creates HDF5 Table", "[hdf5]") {

    const auto path = fs::temp_directory_path();

    const std::vector<sample_row> rows = {
            {"Row 1", 10, 1.3},
            {"Row 2", 10, 1.3},
            {"Row 3", 10, 1.3},
    };

    const auto t = io::HDF5Table<sample_row>(path.string() + "/test.h5", "Test Table", "Test Table");
    t.createTable();
    t.writeRows(rows);
    const auto read = t.readRows();

    REQUIRE_THAT(read, Catch::Equals<sample_row>(rows));
}