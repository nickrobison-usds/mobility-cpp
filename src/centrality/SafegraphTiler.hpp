//
// Created by Nicholas Robison on 7/29/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHTILER_HPP
#define MOBILITY_CPP_SAFEGRAPHTILER_HPP

#include <map-tile/coordinates/Coordinate3D.hpp>
#include <map-tile/ctx/Context.hpp>
#include <shared/data.hpp>


class SafegraphTiler {
public:
    void receive(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx,
                 const mt::coordinates::Coordinate3D &key,
                 const v2 &value);

    void compute();
};


#endif //MOBILITY_CPP_SAFEGRAPHTILER_HPP
