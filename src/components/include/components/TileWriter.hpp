//
// Created by Nicholas Robison on 6/5/20.
//

#ifndef MOBILITY_CPP_TILEWRITER_HPP
#define MOBILITY_CPP_TILEWRITER_HPP

#include "components/detail/CBGOffsetCalculator.hpp"
#include <io/parquet.hpp>
#include <blaze/math/CompressedVector.h>
#include <date/date.h>
#include <string>
#include <utility>

namespace components {
    class TileWriter {
    public:
        TileWriter(const std::string& filename, detail::CBGOffsetCalculator oc);
        arrow::Status writeResults(const date::sys_days &result_date, const blaze::CompressedVector<double, blaze::rowVector> &results, const blaze::CompressedVector<double, blaze::rowVector> &norm_results, const blaze::CompressedVector<std::uint32_t, blaze::rowVector> &visits);
    private:
        const io::Parquet _p;
        const detail::CBGOffsetCalculator _offset_calculator;
        arrow::StringBuilder _cbg_builder;
        arrow::Date32Builder _date_builder;
        arrow::DoubleBuilder _risk_builder;
        arrow::DoubleBuilder _normalize_risk_builder;
        arrow::UInt32Builder _visit_builder;
    };
}


#endif //MOBILITY_CPP_TILEWRITER_HPP
