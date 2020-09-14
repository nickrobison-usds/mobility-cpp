//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHCOUNTYTILER_HPP
#define MOBILITY_CPP_SAFEGRAPHCOUNTYTILER_HPP

#include <map-tile/coordinates/Coordinate3D.hpp>
#include <map-tile/ctx/Context.hpp>
#include <shared/data.hpp>

class SafegraphCountyTiler {
public:
    void receive(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx,
                 const mt::coordinates::Coordinate3D &key,
                 const county_visit &value);

    void compute(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx);
};


#endif //MOBILITY_CPP_SAFEGRAPHCOUNTYTILER_HPP
