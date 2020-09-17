//
// Created by Nicholas Robison on 9/17/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHDWELLMAPPER_HPP
#define MOBILITY_CPP_SAFEGRAPHDWELLMAPPER_HPP


#include "DwellTimes.hpp"
#include <components/detail/CountyOffsetCalculator.hpp>
#include <components/CountyShapefileWrapper.hpp>
#include <map-tile/ctx/Context.hpp>
#include <map-tile/coordinates/Coordinate2D.hpp>
#include <shared/data.hpp>
#include <shared/TileConfiguration.hpp>

class SafegraphDwellMapper {

public:
    void setup(const mt::ctx::MapContext<dwell_times, mt::coordinates::Coordinate2D> &ctx);
    void map(const mt::ctx::MapContext<dwell_times, mt::coordinates::Coordinate2D> &ctx, const std::string &info) const;

private:
    components::TileConfiguration _tc;
    date::sys_days _begin_date;
    date::sys_days _end_date;
    std::unique_ptr<components::detail::CountyOffsetCalculator> _oc;
    std::unique_ptr<components::CountyShapefileWrapper> _c_wrapper;

};


#endif //MOBILITY_CPP_SAFEGRAPHDWELLMAPPER_HPP
