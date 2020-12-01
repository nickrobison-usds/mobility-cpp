//
// Created by Nicholas Robison on 7/29/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHCBGTILER_HPP
#define MOBILITY_CPP_SAFEGRAPHCBGTILER_HPP

#include <map-tile/coordinates/Coordinate3D.hpp>
#include <components/detail/CBGOffsetCalculator.hpp>
#include <components/TemporalGraphs.hpp>
#include <components/TemporalMatricies.hpp>
#include <components/CBGShapefileWrapper.hpp>
#include <map-tile/ctx/Context.hpp>
#include <shared/data.hpp>

class SafegraphCBGTiler {

public:
    void setup(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx);

    void receive(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx,
                 const mt::coordinates::Coordinate3D &key,
                 const v2 &value);

    void compute(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx);

    std::vector<cbg_centrality>
    reduce(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) const;

private:
    void populate_graph(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx);
    void write_parquet(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) const;

    std::unique_ptr<components::detail::CBGOffsetCalculator> _oc;
    std::unique_ptr<components::CBGShapefileWrapper> _s;
    std::unique_ptr<components::TemporalMatricies> _tm;
    std::unique_ptr<components::TemporalGraphs> _graphs;
    components::TileConfiguration _tc;
    date::sys_days _start_date;
    std::vector<std::vector<v2>> _staging;
};


#endif //MOBILITY_CPP_SAFEGRAPHCBGTILER_HPP
