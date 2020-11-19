//
// Created by Nick Robison on 11/19/20.
//

#include "components/TileWriter2.hpp"
#include "spdlog/spdlog.h"

namespace components {

    TileWriter2::TileWriter2(const std::string &filename, detail::CBGOffsetCalculator oc) : _p(io::Parquet(filename)),
                                                                                          _offset_calculator(std::move(oc)) {

    }

    arrow::Status TileWriter2::writeResults(const date::sys_days &result_date,
                                            const blaze::CompressedVector<double, blaze::rowVector> &results,
                                            const blaze::CompressedVector<double, blaze::rowVector> &norm_results,
                                            const blaze::CompressedVector<std::uint32_t, blaze::rowVector> &visits) {
        const std::lock_guard<std::mutex> lock(_mtx);
        arrow::Status status;

        for (size_t i = 0; i < results.size(); i++) {
            // Reverse lookup the index with the matching CBG
            const auto cbg = _offset_calculator.from_local_offset(i);
            if (cbg.has_value()) {
                // Write it out
                status = _cbg_builder.Append(*cbg);
                status = _date_builder.Append(result_date.time_since_epoch().count());
                status = _risk_builder.Append(results[i]);
                status = _normalize_risk_builder.Append(norm_results[i]);
                status = _visit_builder.Append(visits[i]);
            } else {
                spdlog::error("Cannot process index: `{}`", i);
            }
        }

        return status;
    }

    arrow::Status TileWriter2::writeToDisk() {
        arrow::Status status;
        // Convert to table pass to parquet writer
        std::shared_ptr<arrow::Array> cbg_array;
        status = _cbg_builder.Finish(&cbg_array);
        std::shared_ptr<arrow::Array> date_array;
        status = _date_builder.Finish(&date_array);
        std::shared_ptr<arrow::Array> risk_array;
        status = _risk_builder.Finish(&risk_array);
        std::shared_ptr<arrow::Array> norm_risk_array;
        status = _normalize_risk_builder.Finish(&norm_risk_array);
        std::shared_ptr<arrow::Array> visit_array;
        status = _visit_builder.Finish(&visit_array);

        auto schema = arrow::schema(
                {arrow::field("cbg", arrow::utf8()),
                 arrow::field("date", arrow::date32()),
                 arrow::field("risk", arrow::float64()),
                 arrow::field("norm_risk", arrow::float64()),
                 arrow::field("visits", arrow::uint32()),
                });

        auto table = arrow::Table::Make(schema, {cbg_array, date_array, risk_array, norm_risk_array, visit_array});
        return _p.write(*table);
    }

}