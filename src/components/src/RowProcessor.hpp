//
// Created by Nicholas Robison on 6/9/20.
//

#ifndef MOBILITY_CPP_ROWPROCESSOR_HPP
#define MOBILITY_CPP_ROWPROCESSOR_HPP

#include <components/constants.hpp>
#include <components/JoinedLocation.hpp>
#include <components/ShapefileWrapper.hpp>
#include <components/data.hpp>
#include "OffsetCalculator.hpp"
#include "TemporalMatricies.hpp"
#include "TileConfiguration.hpp"

#include <hpx/lcos/future.hpp>
#include <utility>

namespace components {
    class RowProcessor {

    public:
        RowProcessor(const TileConfiguration &conf, JoinedLocation l, ShapefileWrapper s, detail::OffsetCalculator oc,
                     const date::sys_days &start_date) : _conf(conf), _l(std::move(l)), _s(std::move(s)), _matricies(
                {conf._time_count, conf._cbg_max - conf._cbg_min, MAX_CBG}), _start_date(start_date),
                                                         _offset_calculator(std::move(oc)) {
            // Not used
        };

        hpx::future<void> process_row(const shared_ptr<weekly_pattern> &row);

        TemporalMatricies &get_matricies();

    private:
        const TileConfiguration _conf;
        const JoinedLocation _l;
        const ShapefileWrapper _s;
        const detail::OffsetCalculator _offset_calculator;
        const date::sys_days _start_date;
        TemporalMatricies _matricies;

        hpx::future<void>
        handle_row(const std::shared_ptr<weekly_pattern> &row, const std::shared_ptr<joined_location> &jl);

        absl::flat_hash_map<std::string, OGRPoint>
        get_centroid_map(const std::shared_ptr<std::vector<std::pair<std::string, std::uint16_t>>> &visits);

        void insert_rows(std::vector<v2> &rows);
    };
}


#endif //MOBILITY_CPP_ROWPROCESSOR_HPP
