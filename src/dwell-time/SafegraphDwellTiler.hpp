//
// Created by Nicholas Robison on 9/17/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHDWELLTILER_HPP
#define MOBILITY_CPP_SAFEGRAPHDWELLTILER_HPP

#include <map-tile/coordinates/Coordinate2D.hpp>
#include <map-tile/ctx/Context.hpp>
#include <python/PandasEngine.hpp>
#include <shared/data.hpp>


class SafegraphDwellTiler {
public:
    void setup(const mt::ctx::ReduceContext<dwell_times, mt::coordinates::Coordinate2D> &ctx);

    void receive(const mt::ctx::ReduceContext<dwell_times, mt::coordinates::Coordinate2D> &ctx,
                 const mt::coordinates::Coordinate2D &key,
                 const dwell_times &value);

    void compute(const mt::ctx::ReduceContext<dwell_times, mt::coordinates::Coordinate2D> &ctx);

private:
    std::unique_ptr<python::PandasEngine<dwell_times> _engine;
};


#endif //MOBILITY_CPP_SAFEGRAPHDWELLTILER_HPP
