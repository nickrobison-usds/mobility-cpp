//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHCOUNTYMAPPER_HPP
#define MOBILITY_CPP_SAFEGRAPHCOUNTYMAPPER_HPP


#include <components/BaseSafegraphMapper.hpp>
#include <components/CountyShapefileWrapper.hpp>
#include <components/detail/CountyOffsetCalculator.hpp>
#include <shared/data.hpp>

class SafegraphCountyMapper: public components::BaseSafegraphMapper<SafegraphCountyMapper, county_visit> {

public:
    void setup_impl(const mt::ctx::MapContext<county_visit, mt::coordinates::Coordinate3D> &ctx);
    void map_impl(const mt::ctx::MapContext<county_visit, mt::coordinates::Coordinate3D> &ctx, const v2 &row) const;

private:
    std::unique_ptr<components::detail::CountyOffsetCalculator> _oc;
    std::unique_ptr<components::CountyShapefileWrapper> _c_wrapper;
};


#endif //MOBILITY_CPP_SAFEGRAPHCOUNTYMAPPER_HPP
