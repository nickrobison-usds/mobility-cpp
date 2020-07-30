//
// Created by Nicholas Robison on 7/29/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHMAPPER_HPP
#define MOBILITY_CPP_SAFEGRAPHMAPPER_HPP

#include <components/JoinedLocation.hpp>
#include <components/RowProcessor.hpp>
#include <components/ShapefileWrapper.hpp>
#include <map-tile/ctx/Context.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <shared/data.hpp>
#include <string>


class SafegraphMapper {

public:
    void setup(const mt::ctx::MapContext<data_row, mt::coordinates::Coordinate3D> &ctx);
    void map(const mt::ctx::MapContext<data_row, mt::coordinates::Coordinate3D> &ctx, const std::string &info) const;

private:
    std::unique_ptr<components::RowProcessor> _processor;
    std::unique_ptr<components::JoinedLocation> _l;
    std::unique_ptr<components::ShapefileWrapper> _s;
};


#endif //MOBILITY_CPP_SAFEGRAPHMAPPER_HPP
