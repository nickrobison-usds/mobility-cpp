//
// Created by Nicholas Robison on 9/14/20.
//

#include "SafegraphCountyMapper.hpp"

void SafegraphCountyMapper::setup_impl(const mt::ctx::MapContext<county_visit, mt::coordinates::Coordinate3D> &ctx) {
    const auto county_path = ctx.get_config_value("county_path");
    _c_wrapper = std::make_unique<components::CountyShapefileWrapper>(*county_path);
    _oc = std::make_unique<components::detail::CountyOffsetCalculator>(_c_wrapper->build_offsets().get(), _tc);
}

void SafegraphCountyMapper::map_impl(const mt::ctx::MapContext<county_visit, mt::coordinates::Coordinate3D> &ctx, const v2 &row) const {
    // Convert to county_visit
    const county_visit cv{
        row.safegraph_place_id,
        row.visit_date,
        row.location_cbg.substr(0, 5),
        row.visit_cbg.substr(0, 5),
        row.visits,
    };

    const auto global_temporal = cv.visit_date - date::days{this->_tc._time_offset};
    const auto g_count = global_temporal.time_since_epoch().count();
    const auto loc_offset = this->_oc->to_global_offset(cv.location_fips);
    const auto visit_offset = this->_oc->to_global_offset(cv.visit_fips);
    if (visit_offset.has_value() && loc_offset.has_value() && g_count >= 0) {
        // Call nested map
        ctx.emit(mt::coordinates::Coordinate3D(g_count, *loc_offset, *visit_offset), cv);
    }
}


