//
// Created by Nicholas Robison on 6/17/20.
//

#include "components/VisitMatrixWriter.hpp"
#include "spdlog/spdlog.h"

#include <utility>

namespace components {
    VisitMatrixWriter::VisitMatrixWriter(const std::string &filename, detail::OffsetCalculator oc) : _p(
            io::Parquet(filename)), _offset_calculator(std::move(oc)) {
        // Not used
    }

    arrow::Status VisitMatrixWriter::writeResults(const date::sys_days &result_date, const visit_matrix &matrix) {
        arrow::Status status;

        for (size_t i = 0; i < matrix.columns(); i++) {
            const auto poi_cbg = _offset_calculator.cbg_from_local_offset(i);
            if (!poi_cbg.has_value()) {
                spdlog::error("Cannot process source cbg: `{}`", i);
                continue;
            }
            for (visit_matrix::ConstIterator it = matrix.cbegin(i); it != matrix.cend(i); ++it) {
                const auto dist = std::distance(matrix.cbegin(i), it);
                const auto visitor_cbg = _offset_calculator.cbg_from_offset(dist);
                if (!visitor_cbg.has_value()) {
                    spdlog::error("Cannot process dest cbg: `{}`", dist);
                    continue;
                }
                status = _poi_cbg_builder.Append(*poi_cbg);
                status = _visitor_cbg_builder.Append(*visitor_cbg);
                status = _date_builder.Append(result_date.time_since_epoch().count());
                status = _visit_builder.Append(it->value());
            }
        }

        // Convert to table pass to parquet writer
        std::shared_ptr<arrow::Array> poi_cbg_array;
        status = _poi_cbg_builder.Finish(&poi_cbg_array);
        std::shared_ptr<arrow::Array> visitor_cbg_array;
        status = _visitor_cbg_builder.Finish(&visitor_cbg_array);
        std::shared_ptr<arrow::Array> date_array;
        status = _date_builder.Finish(&date_array);
        std::shared_ptr<arrow::Array> visit_array;
        status = _visit_builder.Finish(&visit_array);

        auto schema = arrow::schema(
                {arrow::field("poi_cbg", arrow::utf8()),
                 arrow::field("visitor_cbg", arrow::utf8()),
                 arrow::field("date", arrow::date32()),
                 arrow::field("visits", arrow::uint32()),
                });

        auto table = arrow::Table::Make(schema, {poi_cbg_array, visitor_cbg_array, date_array, visit_array});
        return _p.write(*table);
    }
}