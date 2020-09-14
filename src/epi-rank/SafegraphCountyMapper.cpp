//
// Created by Nicholas Robison on 9/14/20.
//

#include "SafegraphCountyMapper.hpp"

void SafegraphCountyMapper::setup_impl(const mt::ctx::MapContext<county_visit, mt::coordinates::Coordinate3D> &ctx) {
    // Not used
}

void SafegraphCountyMapper::map_impl(const mt::ctx::MapContext<county_visit, mt::coordinates::Coordinate3D> &ctx,
                                     const std::size_t g_count, const std::size_t loc_offset,
                                     const std::size_t visit_offset, const v2 &row) const {

    // Roll up to county and do the emit

}


