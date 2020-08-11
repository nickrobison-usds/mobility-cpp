//
// Created by Nicholas Robison on 7/29/20.
//

#ifndef MOBILITY_CPP_SAFEGRAPHMAPPER_HPP
#define MOBILITY_CPP_SAFEGRAPHMAPPER_HPP

#include <components/JoinedLocation.hpp>
#include <components/OffsetCalculator.hpp>
#include <components/ShapefileWrapper.hpp>
#include <map-tile/ctx/Context.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <shared/data.hpp>
#include <string>


class SafegraphMapper {

public:
    void setup(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx);
    void map(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx, const std::string &info) const;

private:
    std::vector<v2> process_row(const weekly_pattern &row) const;
    std::vector<v2> handle_row(const weekly_pattern &row, const std::shared_ptr<joined_location> &jl) const;
    absl::flat_hash_map<std::string, OGRPoint>
    get_centroid_map(const std::vector<std::pair<std::string, std::uint16_t>> &visits) const;

    std::unique_ptr<components::JoinedLocation> _l;
    std::unique_ptr<components::ShapefileWrapper> _s;
    std::unique_ptr<components::detail::OffsetCalculator> _oc;
    components::TileConfiguration _tc;
    date::sys_days _begin_date;
    date::sys_days _end_date;
};


#endif //MOBILITY_CPP_SAFEGRAPHMAPPER_HPP
