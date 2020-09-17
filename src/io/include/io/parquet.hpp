#ifndef PARQUET_H
#define PARQUET_H

#include <iostream>
#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/exception.h>
#include "spdlog/fmt/ostr.h"
#include "spdlog/fmt/fmt.h"

using namespace std;

namespace io {
    class Parquet {
    public:
        explicit Parquet(string filename, std::size_t rows = 10'000, parquet::Compression::type compressor = parquet::Compression::SNAPPY);

        [[nodiscard]] shared_ptr<arrow::Table> read() const;

        [[nodiscard]] arrow::Status write(const arrow::Table &table, bool append = false) const;

    private:
        const string _filename;
        const std::size_t _rows;
        const parquet::Compression::type _compressor;
    };
}

#endif