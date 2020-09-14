//
// Created by Nicholas Robison on 9/14/20.
//

#include "SafegraphCountyTiler.hpp"

void SafegraphCountyTiler::receive(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx,
                                   const mt::coordinates::Coordinate3D &key, const county_visit &value) {

}

void SafegraphCountyTiler::compute(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx) {

}
