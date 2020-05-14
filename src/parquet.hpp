#ifndef PARQUET_H
#define PARQUET_H

#include <iostream>
#include <arrow/api.h>
#include <arrow/io/api.h>

#include "spdlog/fmt/ostr.h"
#include "spdlog/fmt/fmt.h"

using ArrowDate = arrow::Date32Type::c_type;

struct data_row
{
    const std::string location_cbg;
    const std::string visit_cbg;
    const ArrowDate date;
    std::vector<int16_t> visits;
    const double distance;

    template <typename OStream>
    friend OStream &
    operator<<(OStream &o, const data_row &dr)
    {
        auto msg = fmt::format("Location: {}\n", dr.location_cbg);
        return o << msg;
    }
};

struct visit_row
{
    const std::string location_cbg;
    const std::string visit_cbg;
    const ArrowDate date;
    const int16_t visits;
    const double distance;
    const double weighted_total;
};

class Parquet
{
public:
    Parquet(const std::string &filename);
    std::shared_ptr<arrow::Table> read() const;
    arrow::Status write(const arrow::Table &table) const;

private:
    const std::string _filename;
};

#endif