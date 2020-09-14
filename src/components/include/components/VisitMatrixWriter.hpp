//
// Created by Nicholas Robison on 6/17/20.
//

#ifndef MOBILITY_CPP_VISITMATRIXWRITER_HPP
#define MOBILITY_CPP_VISITMATRIXWRITER_HPP

#include "components/CBGOffsetCalculator.hpp"
#include "components/TemporalMatricies.hpp"
#include <io/parquet.hpp>
#include <date/date.h>

namespace components {
    class VisitMatrixWriter {
    public:
        VisitMatrixWriter(const std::string &filename, detail::CBGOffsetCalculator oc);
        arrow::Status writeResults(const date::sys_days &result_date, const visit_matrix &matrix);

    private:
        const io::Parquet _p;
        const detail::CBGOffsetCalculator _offset_calculator;
        arrow::StringBuilder _poi_cbg_builder;
        arrow::StringBuilder _visitor_cbg_builder;
        arrow::Date32Builder _date_builder;
        arrow::UInt32Builder _visit_builder;
    };
}


#endif //MOBILITY_CPP_VISITMATRIXWRITER_HPP
