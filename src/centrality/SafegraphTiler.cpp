//
// Created by Nicholas Robison on 7/29/20.
//

#include "SafegraphTiler.hpp"
#include <shared/DateUtils.hpp>
#include <shared/ConversionUtils.hpp>
#include <spdlog/spdlog.h>

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
    spdlog::debug("Call compute.");

    for (std::size_t i = 0; i < _tc._time_count; i++) {
        const auto matrix_pair = _tm->get_matrix_pair(i);
        const distance_matrix result = _tm->compute(i);

//        const blaze::CompressedVector<double, blaze::rowVector> cbg_risk_score = blaze::sum<blaze::columnwise>(
//                result);
//        const blaze::CompressedVector<std::uint32_t, blaze::rowVector> visit_sum = blaze::sum<blaze::columnwise>(
//                matrix_pair.vm);
    }
}