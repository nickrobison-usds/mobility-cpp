//
// Created by Nicholas Robison on 9/14/20.
//

#include "SafegraphCBGMapper.hpp"
#include <components/CBGOffsetCalculator.hpp>

void SafegraphCBGMapper::setup_impl(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx) {
    _oc = std::make_unique<components::detail::CBGOffsetCalculator>(
            components::detail::CBGOffsetCalculator(_s->build_offsets().get(), _tc));
}

void SafegraphCBGMapper::map_impl(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx, const v2 &row) const {
    const auto global_temporal = row.visit_date - date::days{this->_tc._time_offset};
    const auto g_count = global_temporal.time_since_epoch().count();
    const auto loc_offset = this->_oc->to_global_offset(row.location_cbg);
    const auto visit_offset = this->_oc->to_global_offset(row.visit_cbg);
    if (visit_offset.has_value() && loc_offset.has_value() && g_count >= 0) {
        // Call nested map
        ctx.emit(mt::coordinates::Coordinate3D(g_count, *loc_offset, *visit_offset), row);
    }
}
