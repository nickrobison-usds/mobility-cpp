//
// Created by Nicholas Robison on 9/17/20.
//

#include "SafegraphDwellMapper.hpp"
#include <shared/ConversionUtils.hpp>
#include <shared/DateUtils.hpp>


void SafegraphDwellMapper::setup(const mt::ctx::MapContext<dwell_times, mt::coordinates::Coordinate2D> &ctx) {
    spdlog::debug("Mapper setup initializing");
    const auto cbg_path = ctx.get_config_value("cbg_path");
    const auto poi_path = ctx.get_config_value("poi_path");
    const auto start_date_string = ctx.get_config_value("start_date");
    const shared::days start_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*start_date_string));
    const auto end_date_string = ctx.get_config_value("end_date");
    const shared::days end_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*end_date_string));

    const auto sd = chrono::floor<date::days>(start_date);


    // Force cast to sys_days
    const auto ff = chrono::floor<date::days>(start_date);
    _begin_date = date::sys_days(start_date);
    _end_date = date::sys_days(end_date);


    const auto time_bounds = chrono::duration_cast<shared::days>(end_date - start_date).count();

    _tc._nr = 16;
    // These values are really confusing
    _tc._time_offset = (_begin_date + date::days{ctx.get_tile().min_corner().get_dim0()}).time_since_epoch().count();
    _tc._time_count = time_bounds;
    _tc._tile_min = ctx.get_tile().min_corner().get_dim1();
    _tc._tile_max = ctx.get_tile().max_corner().get_dim1();

    spdlog::debug("Mapper setup complete.");
}

void SafegraphDwellMapper::map(const mt::ctx::MapContext<dwell_times, mt::coordinates::Coordinate2D> &ctx,
                               const string &info) const {

}
