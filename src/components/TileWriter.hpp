//
// Created by Nicholas Robison on 6/5/20.
//

#ifndef MOBILITY_CPP_TILEWRITER_HPP
#define MOBILITY_CPP_TILEWRITER_HPP

#include <io/parquet.hpp>
#include <blaze/math/CompressedVector.h>
#include <boost/bimap.hpp>
#include <date/date.h>
#include <string>
#include <utility>

namespace components {
    class TileWriter {
    public:
        typedef boost::bimap<std::string, std::size_t> offset_bimap;
        TileWriter(const std::string& filename, const offset_bimap &map);
        arrow::Status writeResults(const date::sys_days &result_date, const blaze::CompressedVector<double> &results);
    private:
        const io::Parquet _p;
        const offset_bimap _map;
        arrow::StringBuilder _cbg_builder;
        arrow::Date32Builder _date_builder;
        arrow::DoubleBuilder _risk_builder;
        arrow::DoubleBuilder _normalize_risk_builder;
    };
}


#endif //MOBILITY_CPP_TILEWRITER_HPP
