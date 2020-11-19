//
// Created by Nick Robison on 11/19/20.
//

#pragma once

#include "components/detail/CBGOffsetCalculator.hpp"
#include "components/TemporalMatricies.hpp"
#include <io/parquet.hpp>
#include <date/date.h>
#include <mutex>

namespace components {
    class VisitMatrixWriter2 {
    public:
        VisitMatrixWriter2(const std::string &filename, detail::CBGOffsetCalculator oc);

        arrow::Status writeResults(const date::sys_days &result_date, const visit_matrix &matrix);
        arrow::Status writeToDisk();

    private:
        const io::Parquet _p;
        const detail::CBGOffsetCalculator _offset_calculator;
        arrow::StringBuilder _poi_cbg_builder;
        arrow::StringBuilder _visitor_cbg_builder;
        arrow::Date32Builder _date_builder;
        arrow::UInt32Builder _visit_builder;
        std::mutex _mtx;
    };
}
