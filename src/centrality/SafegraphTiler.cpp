//
// Created by Nicholas Robison on 7/29/20.
//

#include "SafegraphTiler.hpp"
#include <boost/filesystem.hpp>
#include <blaze/math/CompressedVector.h>
#include <blaze/math/DynamicVector.h>
#include <components/TileWriter.hpp>
#include <components/VisitMatrixWriter.hpp>
#include <shared/DateUtils.hpp>
#include <shared/ConversionUtils.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace fs = boost::filesystem;

void print_timing(const std::string &op_name, const std::uint64_t elapsed) {
    const chrono::nanoseconds n{elapsed};
    spdlog::debug("[{}] took {} ms", op_name, chrono::duration_cast<chrono::milliseconds>(n).count());
}

void SafegraphTiler::setup(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) {
    const auto cbg_path = ctx.get_config_value("cbg_path");
    const auto poi_path = ctx.get_config_value("poi_path");
    const auto start_date_string = ctx.get_config_value("start_date");
    const shared::days start_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*start_date_string));
    const auto end_date_string = ctx.get_config_value("end_date");
    const shared::days end_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*end_date_string));

    const auto sd = chrono::floor<date::days>(start_date);

    // Force cast to sys_days
    const auto ff = chrono::floor<date::days>(start_date);
    const date::sys_days d(start_date);
    const date::sys_days e(end_date);

    _s = std::make_unique<components::ShapefileWrapper>(components::ShapefileWrapper(*cbg_path));

    _tc._nr = 16;
    // These values are really confusing
    _tc._time_offset = (d + date::days{ctx.get_tile().min_corner().get_dim0()}).time_since_epoch().count();
    _tc._time_count = ctx.get_tile().max_corner().get_dim0() - ctx.get_tile().min_corner().get_dim0();
    _tc._cbg_min = ctx.get_tile().min_corner().get_dim1();
    _tc._cbg_max = ctx.get_tile().max_corner().get_dim1();
    _oc = std::make_unique<components::detail::OffsetCalculator>(
            components::detail::OffsetCalculator(_s->build_offsets().get(), _tc));

    const auto loc_dims = ctx.get_tile().max_corner().get_dim1() - ctx.get_tile().min_corner().get_dim1();
    const auto visit_dims = ctx.get_tile().max_corner().get_dim2() - ctx.get_tile().min_corner().get_dim2();

    // Initialize the Temporal Matricies
    _tm = std::make_unique<components::TemporalMatricies>(_tc._time_count, loc_dims, visit_dims);
    spdlog::debug("Tiler setup complete.");

}

void SafegraphTiler::receive(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx,
                             const mt::coordinates::Coordinate3D &key,
                             const v2 &value) {
    // Convert back to local coordinate space
    const auto local_temporal = value.visit_date - date::days{_tc._time_offset};
    const auto x_idx = _oc->calculate_local_offset(value.location_cbg);
    const auto y_idx = _oc->calculate_cbg_offset(value.visit_cbg);
    if (y_idx.has_value()) {
        _tm->insert(local_temporal.time_since_epoch().count(), x_idx, *y_idx, value.visits, value.distance);
    } else {
        spdlog::error("Visitor CBG {} is not in map, skipping insert", value.visit_cbg);
    }
}

void SafegraphTiler::compute(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) {
    write_parquet(ctx);
}

void SafegraphTiler::write_parquet(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) const {
    const auto output_name = ctx.get_config_value("output_name");
    const auto output_dir = ctx.get_config_value("output_dir");

    // Figure out my temporal start date
    const date::sys_days start_date = date::sys_days{} + date::days(_tc._time_offset);
    for (std::size_t i = 0; i < _tc._time_count; i++) {
// Some nice pretty-printing of the dates
        const date::sys_days matrix_date = start_date + date::days{i};
        const auto parquet_filename = fmt::format("{}-{}-{}-{}.parquet",
                                                  *output_name,
                                                  *(_oc->cbg_from_offset(_tc._cbg_min)),
                                                  *(_oc->cbg_from_offset(_tc._cbg_max)),
                                                  date::format("%F", matrix_date));
        const auto p_file = fs::path(*output_dir) /= fs::path(parquet_filename);

        const auto visit_filename = fmt::format("{}-visits-{}-{}-{}.parquet",
                                                *output_name,
                                                *(_oc->cbg_from_offset(_tc._cbg_min)),
                                                *(_oc->cbg_from_offset(_tc._cbg_max)),
                                                date::format("%F", matrix_date));

        const auto v_file = fs::path(*output_dir) /= fs::path(visit_filename);

        components::TileWriter tw(std::string(p_file.string()), *_oc);
        components::VisitMatrixWriter vw(std::string(v_file.string()), *_oc);

        const auto multiply_start = hpx::util::high_resolution_clock::now();
        const auto matrix_pair = _tm->get_matrix_pair(i);
        const distance_matrix result = _tm->compute(i);

        // Sum the total risk for each cbg
        const blaze::CompressedVector<double, blaze::rowVector> cbg_risk_score = blaze::sum<blaze::columnwise>(
                result);
        const double max = blaze::max(cbg_risk_score);
        const blaze::CompressedVector<std::uint32_t, blaze::rowVector> visit_sum = blaze::sum<blaze::columnwise>(
                matrix_pair.vm);

        // scale it back down
        spdlog::info("Performing multiplication for {}", date::format("%F", matrix_date));
        const auto multiply_elapsed = hpx::util::high_resolution_clock::now() - multiply_start;
        print_timing("Multiply", multiply_elapsed);

        spdlog::info("Beginning tile write");
        const auto write_start = hpx::util::high_resolution_clock::now();
        arrow::Status status = tw.writeResults(matrix_date, cbg_risk_score, {}, visit_sum);
        if (!status.ok()) {
            spdlog::critical("Could not write parquet file: {}", status.CodeAsString());
        }
        status = vw.writeResults(matrix_date, matrix_pair.vm);
        if (!status.ok()) {
            spdlog::critical("Could not write parquet file: {}", status.CodeAsString());
        }
        const auto write_elapsed = hpx::util::high_resolution_clock::now() - write_start;
        print_timing("File Write", write_elapsed);
    }

}