//
// Created by Nicholas Robison on 7/29/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHMAPPER_HPP
#define MOBILITY_CPP_SAFEGRAPHMAPPER_HPP

#include <map-tile/ctx/Context.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <shared/data.hpp>
#include <string>


class SafegraphMapper {

public:
    void map(const mt::ctx::MapContext<data_row, mt::coordinates::Coordinate3D> &ctx, const std::string &info) const;
};


#endif //MOBILITY_CPP_SAFEGRAPHMAPPER_HPP
