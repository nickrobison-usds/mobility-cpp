//
// Created by Nicholas Robison on 6/9/20.
//

#ifndef MOBILITY_CPP_ROWPROCESSOR_HPP
#define MOBILITY_CPP_ROWPROCESSOR_HPP

#include <components/constants.hpp>
#include <components/JoinedLocation.hpp>
#include <components/ShapefileWrapper.hpp>
#include <components/data.hpp>
#include "TemporalMatricies.hpp"
#include "TileConfiguration.hpp"
#include "types.hpp"

#include <hpx/lcos/future.hpp>
#include <utility>

namespace components {
    class RowProcessor {

    public:
        RowProcessor(const TileConfiguration &conf, JoinedLocation l, ShapefileWrapper s, const detail::offset_bimap &offset_map, const date::sys_days &start_date): _conf(conf), _l(std::move(l)), _s(std::move(s)), _matricies(
                {conf._time_count, MAX_CBG}), _start_date(start_date), _offset_map(offset_map) {
            // Not used
        };
        hpx::future<void> process_row(const shared_ptr<weekly_pattern> row);
        TemporalMatricies& get_matricies();

    private:
        const TileConfiguration _conf;
        const JoinedLocation _l;
        const ShapefileWrapper _s;
        const detail::offset_bimap _offset_map;
        const date::sys_days _start_date;
        TemporalMatricies _matricies;
        hpx::future<void> handle_row(const std::shared_ptr<weekly_pattern> row, const std::shared_ptr<joined_location> jl);
        hpx::future<absl::flat_hash_map<std::string, OGRPoint>> get_centroid_map(const std::shared_ptr<std::vector<std::pair<std::string, std::uint16_t>>> visits);
        hpx::future<void> insert_rows(hpx::future<std::vector<v2>> rows);
        std::size_t calculate_cbg_offset(const std::string &cbg_code) const;
    };
}


#endif //MOBILITY_CPP_ROWPROCESSOR_HPP
