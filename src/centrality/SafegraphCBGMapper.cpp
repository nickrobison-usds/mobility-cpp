//
// Created by Nicholas Robison on 9/14/20.
//

#include "SafegraphCBGMapper.hpp"

void SafegraphCBGMapper::setup_impl(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx) {
    // Not used
}

void SafegraphCBGMapper::map_impl(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx, const std::size_t g_count,  const std::size_t loc_offset,  const std::size_t visit_offset, const v2 &row) const {
    ctx.emit(mt::coordinates::Coordinate3D(g_count, loc_offset, visit_offset), row);
}
