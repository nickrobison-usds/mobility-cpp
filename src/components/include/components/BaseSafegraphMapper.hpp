//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_BASESAFEGRAPHMAPPER_HPP
#define MOBILITY_CPP_BASESAFEGRAPHMAPPER_HPP

#include "JoinedLocation.hpp"
#include "OffsetCalculator.hpp"
#include "ShapefileWrapper.hpp"
#include <map-tile/ctx/Context.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <shared/ConversionUtils.hpp>
#include <shared/DateUtils.hpp>
#include <shared/data.hpp>
#include <string_view>
#include <vector>

namespace components {

    template<class Backend, class Output>
    class BaseSafegraphMapper {

    public:
        BaseSafegraphMapper() = default;
        void setup(const mt::ctx::MapContext<Output, mt::coordinates::Coordinate3D> &ctx) {
            spdlog::debug("Mapper setup initializing");
            const auto cbg_path = ctx.get_config_value("cbg_path");
            const auto poi_path = ctx.get_config_value("poi_path");
            const auto start_date_string = ctx.get_config_value("start_date");
            const shared::days start_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*start_date_string));
            const auto end_date_string = ctx.get_config_value("end_date");
            const shared::days end_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*end_date_string));

            const auto sd = chrono::floor<date::days>(start_date);


            // Force cast to sys_days
            const auto ff = chrono::floor<date::days>(start_date);
            _begin_date = date::sys_days(start_date);
            _end_date = date::sys_days(end_date);


            const auto time_bounds = chrono::duration_cast<shared::days>(end_date - start_date).count();
//
            _l = std::make_unique<components::JoinedLocation>(components::JoinedLocation({}, *poi_path, *poi_path));
            _s = std::make_unique<components::ShapefileWrapper>(components::ShapefileWrapper(*cbg_path));

            _tc._nr = 16;
            // These values are really confusing
            _tc._time_offset = (_begin_date + date::days{ctx.get_tile().min_corner().get_dim0()}).time_since_epoch().count();
            _tc._time_count = time_bounds;
            _tc._cbg_min = ctx.get_tile().min_corner().get_dim1();
            _tc._cbg_max = ctx.get_tile().max_corner().get_dim1();
            _oc = std::make_unique<components::detail::OffsetCalculator>(
                    components::detail::OffsetCalculator(_s->build_offsets().get(), _tc));

            // Call nested setup
            spdlog::debug("Mapper setup complete.");
        }
        void map(const mt::ctx::MapContext<Output, mt::coordinates::Coordinate3D> &ctx, const std::string &info) const {
            const auto row = parse_string(info);
            if (row.date_range_start < _begin_date || row.date_range_end > _end_date)
                return;

            const auto rows = process_row(row);

            std::for_each(rows.begin(), rows.end(), [&ctx, this](const auto &r) {
                const auto global_temporal = r.visit_date - date::days{this->_tc._time_offset};
                const auto g_count = global_temporal.time_since_epoch().count();
                const auto loc_offset = this->_oc->calculate_cbg_offset(r.location_cbg);
                const auto visit_offset = this->_oc->calculate_cbg_offset(r.visit_cbg);
                if (visit_offset.has_value() && loc_offset.has_value() && g_count >= 0) {
                    // Call nested map
//                    ctx.emit(mt::coordinates::Coordinate3D(g_count, *loc_offset, *visit_offset), r);
                }
            });
        }

    private:
        std::vector<v2> process_row(const weekly_pattern &row) const;
        std::vector<v2> handle_row(const weekly_pattern &row, const std::shared_ptr<joined_location> &jl) const;
        absl::flat_hash_map<std::string, OGRPoint>
        get_centroid_map(const std::vector<std::pair<std::string, std::uint16_t>> &visits) const;
        weekly_pattern parse_string(std::string_view v);

        std::unique_ptr<components::JoinedLocation> _l;
        std::unique_ptr<components::ShapefileWrapper> _s;
        std::unique_ptr<components::detail::OffsetCalculator> _oc;
        components::TileConfiguration _tc;
        date::sys_days _begin_date;
        date::sys_days _end_date;

    };
}

#endif //MOBILITY_CPP_BASESAFEGRAPHMAPPER_HPP