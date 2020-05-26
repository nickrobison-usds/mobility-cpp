#ifndef PARQUET_H
#define PARQUET_H

#include <iostream>
#include <arrow/api.h>
#include <arrow/io/api.h>

#include "spdlog/fmt/ostr.h"
#include "spdlog/fmt/fmt.h"

using namespace std;

namespace io {
    class Parquet {
    public:
        explicit Parquet(string filename);

        [[nodiscard]] shared_ptr<arrow::Table> read() const;

        arrow::Status write(const arrow::Table &table, bool append = false) const;

    private:
        const string _filename;
    };
}

#endif