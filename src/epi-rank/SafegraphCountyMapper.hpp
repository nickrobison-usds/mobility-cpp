//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHCOUNTYMAPPER_HPP
#define MOBILITY_CPP_SAFEGRAPHCOUNTYMAPPER_HPP


#include <components/BaseSafegraphMapper.hpp>
#include <shared/data.hpp>

class SafegraphCountyMapper: public components::BaseSafegraphMapper<SafegraphCountyMapper, county_visit> {

public:
    void setup_impl(const mt::ctx::MapContext<county_visit, mt::coordinates::Coordinate3D> &ctx);
    void map_impl(const mt::ctx::MapContext<county_visit, mt::coordinates::Coordinate3D> &ctx, const std::size_t g_count,  const std::size_t loc_offset,  const std::size_t visit_offset, const v2 &row) const;
};


#endif //MOBILITY_CPP_SAFEGRAPHCOUNTYMAPPER_HPP
