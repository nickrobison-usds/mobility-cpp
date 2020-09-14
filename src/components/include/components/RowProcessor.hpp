//
// Created by Nicholas Robison on 6/9/20.
//

#ifndef MOBILITY_CPP_ROWPROCESSOR_HPP
#define MOBILITY_CPP_ROWPROCESSOR_HPP

#include <shared/constants.hpp>
#include <components/JoinedLocation.hpp>
#include <components/CBGShapefileWrapper.hpp>
#include <shared/data.hpp>
#include "components/detail/CBGOffsetCalculator.hpp"
#include "TemporalMatricies.hpp"
#include "shared/TileConfiguration.hpp"

#include <hpx/future.hpp>
#include <utility>

namespace components {
    class RowProcessor {

    public:
        RowProcessor(const TileConfiguration &conf, const JoinedLocation &l, const CBGShapefileWrapper &s, detail::CBGOffsetCalculator &oc,
                     const date::sys_days &start_date) : _conf(conf), _l(l), _s(s), _matricies(
                {conf._time_count, conf._tile_max - conf._tile_min, shared::MAX_CBG}), _start_date(start_date),
                                                         _offset_calculator(oc) {
            // Not used
        };

        hpx::future<void> process_row(const shared_ptr<weekly_pattern> &row);

        TemporalMatricies &get_matricies();

    private:
        const TileConfiguration _conf;
        const JoinedLocation _l;
        const CBGShapefileWrapper _s;
        const detail::CBGOffsetCalculator _offset_calculator;
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
