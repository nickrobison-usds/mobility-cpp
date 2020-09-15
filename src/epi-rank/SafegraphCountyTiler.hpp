//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHCOUNTYTILER_HPP
#define MOBILITY_CPP_SAFEGRAPHCOUNTYTILER_HPP

#include "BlazeMatricies.hpp"
#include "EigenMatricies.hpp"
#include <components/CountyShapefileWrapper.hpp>
#include <components/detail/CountyOffsetCalculator.hpp>
#include <shared/TileConfiguration.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <map-tile/ctx/Context.hpp>
#include <shared/data.hpp>
#include <utility>

class SafegraphCountyTiler {
public:
    void setup(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx);
    void receive(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx,
                 const mt::coordinates::Coordinate3D &key,
                 const county_visit &value);

    void compute(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx);

private:
    std::unique_ptr<components::detail::CountyOffsetCalculator> _oc;
    std::unique_ptr<components::CountyShapefileWrapper> _c_wrapper;
    components::TileConfiguration _tc;
    date::sys_days _start_date;
    std::unique_ptr<EigenMatricies> _matricies;
    std::unique_ptr<BlazeMatricies> _bm;
};


#endif //MOBILITY_CPP_SAFEGRAPHCOUNTYTILER_HPP
