//
// Created by Nicholas Robison on 6/17/20.
//

#include "VisitMatrixWriter.hpp"
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
            const auto source_cbg = _offset_calculator.cbg_from_local_offset(i);
            if (!source_cbg.has_value()) {
                spdlog::error("Cannot process source cbg: `{}`", i);
                continue;
            }
            for (visit_matrix::ConstIterator it = matrix.cbegin(i); it != matrix.cend(i); ++it) {
                const auto dist = std::distance(matrix.cbegin(i), it);
                const auto dest_cbg = _offset_calculator.cbg_from_offset(dist);
                if (!dest_cbg.has_value()) {
                    spdlog::error("Cannot process dest cbg: `{}`", dist);
                    continue;
                }
                status = _source_cbg_builder.Append(*source_cbg);
                status = _dest_cbg_builder.Append(*dest_cbg);
                status = _date_builder.Append(result_date.time_since_epoch().count());
                status = _visit_builder.Append(it->value());
            }
        }

        // Convert to table pass to parquet writer
        std::shared_ptr<arrow::Array> source_cbg_array;
        status = _source_cbg_builder.Finish(&source_cbg_array);
        std::shared_ptr<arrow::Array> dest_cbg_array;
        status = _dest_cbg_builder.Finish(&dest_cbg_array);
        std::shared_ptr<arrow::Array> date_array;
        status = _date_builder.Finish(&date_array);
        std::shared_ptr<arrow::Array> visit_array;
        status = _visit_builder.Finish(&visit_array);

        auto schema = arrow::schema(
                {arrow::field("source_cbg", arrow::utf8()),
                 arrow::field("dest_cbg", arrow::utf8()),
                 arrow::field("date", arrow::date32()),
                 arrow::field("visits", arrow::uint32()),
                });

        auto table = arrow::Table::Make(schema, {source_cbg_array, dest_cbg_array, date_array, visit_array});
        return _p.write(*table);
    }
}