//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHCBGMAPPER_HPP
#define MOBILITY_CPP_SAFEGRAPHCBGMAPPER_HPP

#include <components/BaseSafegraphMapper.hpp>
#include <map-tile/ctx/Context.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <shared/data.hpp>


class SafegraphCBGMapper: public components::BaseSafegraphMapper<SafegraphCBGMapper, v2> {

public:
    void setup_impl(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx);
    void map_impl(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx, const std::size_t g_count,  const std::size_t loc_offset,  const std::size_t visit_offset, const v2 &row) const;
};


#endif //MOBILITY_CPP_SAFEGRAPHCBGMAPPER_HPP
