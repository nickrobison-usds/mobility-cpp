//
// Created by Nicholas Robison on 6/5/20.
//

#ifndef MOBILITY_CPP_TILEWRITER_HPP
#define MOBILITY_CPP_TILEWRITER_HPP

#include <io/parquet.hpp>
#include <blaze/math/CompressedVector.h>
#include <date/date.h>
#include <string>
#include <utility>
#include "types.hpp"

namespace components {
    class TileWriter {
    public:
        TileWriter(const std::string& filename, const detail::offset_bimap &map);
        arrow::Status writeResults(const date::sys_days &result_date, const blaze::CompressedVector<double> &results, const blaze::CompressedVector<double> &norm_results);
    private:
        const io::Parquet _p;
        const detail::offset_bimap _map;
        arrow::StringBuilder _cbg_builder;
        arrow::Date32Builder _date_builder;
        arrow::DoubleBuilder _risk_builder;
        arrow::DoubleBuilder _normalize_risk_builder;
    };
}


#endif //MOBILITY_CPP_TILEWRITER_HPP
