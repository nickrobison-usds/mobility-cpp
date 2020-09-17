//
// Created by Nicholas Robison on 9/17/20.
//

#include "SafegraphDwellMapper.hpp"
#include <components/detail/helpers.hpp>
#include <date/date.h>
#include <shared/ConversionUtils.hpp>
#include <shared/DateUtils.hpp>
#include <algorithm>

const static std::array<std::string, 5> buckets{"<5", "5-20", "21-60", "61-240", ">240"};


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

    const auto county_path = ctx.get_config_value("county_path");
    _c_wrapper = std::make_unique<components::CountyShapefileWrapper>(*county_path);
    _oc = std::make_unique<components::detail::CountyOffsetCalculator>(_c_wrapper->build_offsets().get(), _tc);

    spdlog::debug("Mapper setup complete.");
}

void SafegraphDwellMapper::map(const mt::ctx::MapContext<dwell_times, mt::coordinates::Coordinate2D> &ctx,
                               const string &info) const {

    const auto pattern = components::detail::parse_string(info);

    std::vector<std::uint16_t> values;
    values.reserve(5);
    // Get the values from the map, we use a static array as a cheap sorting trick.
    std::transform(buckets.begin(), buckets.end(), std::back_inserter(values), [&pattern](const auto &key) {
        return pattern.bucketed_dwell_times.at(key);
    });

    dwell_times dw{
        pattern.safegraph_place_id,
        date::format("%F", pattern.date_range_start),
        pattern.raw_visit_counts,
        pattern.median_dwell_times,
        values,
    };

    const auto global_temporal = pattern.date_range_start - date::days{this->_tc._time_offset};
    const auto g_count = global_temporal.time_since_epoch().count();
    const auto loc_offset = this->_oc->to_global_offset(pattern.poi_cbg.substr(0, 5));
    if (loc_offset.has_value() && g_count >= 0) {
        // Call nested map
        ctx.emit(mt::coordinates::Coordinate2D(g_count, *loc_offset), dw);
    }

}
