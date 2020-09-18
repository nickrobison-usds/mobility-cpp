//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_BASESAFEGRAPHMAPPER_HPP
#define MOBILITY_CPP_BASESAFEGRAPHMAPPER_HPP

#include "JoinedLocation.hpp"
#include "CBGShapefileWrapper.hpp"
#include "detail/helpers.hpp"
#include <map-tile/ctx/Context.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <shared/ConversionUtils.hpp>
#include <shared/DateUtils.hpp>
#include <shared/TileConfiguration.hpp>
#include <string_view>
#include <vector>

namespace components {

    std::vector<v2>
    expandRow(const weekly_pattern &row,
              const std::vector<std::pair<std::string, std::uint16_t>> &cbg_visits);

    std::vector<std::pair<std::string, std::uint16_t>> extract_cbg_visits(const weekly_pattern &row);

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
            _l = std::make_unique<components::JoinedLocation>(components::JoinedLocation({}, *poi_path, *poi_path));
            _s = std::make_unique<components::CBGShapefileWrapper>(components::CBGShapefileWrapper(*cbg_path));

            _tc._nr = 16;
            // These values are really confusing
            _tc._time_offset = (_begin_date + date::days{ctx.get_tile().min_corner().get_dim0()}).time_since_epoch().count();
            _tc._time_count = time_bounds;
            _tc._tile_min = ctx.get_tile().min_corner().get_dim1();
            _tc._tile_max = ctx.get_tile().max_corner().get_dim1();

            // Call nested setup
            static_cast<Backend &> (*this).setup_impl(ctx);
            spdlog::debug("Mapper setup complete.");
        }
        void map(const mt::ctx::MapContext<Output, mt::coordinates::Coordinate3D> &ctx, const std::string &info) const {
            const auto row = detail::parse_string(info);
            if (row.date_range_start < _begin_date || row.date_range_end > _end_date)
                return;

            const auto rows = process_row(row);

            std::for_each(rows.begin(), rows.end(), [&ctx, this](const auto &r) {
                static_cast<Backend const &> (*this).map_impl(ctx, r);
            });
        }

    private:
        [[nodiscard]] std::vector<v2> process_row(const weekly_pattern &row) const {
            return _l->find_location(row.safegraph_place_id).then(
                    [this, row](hpx::future<joined_location> location_future) {
                        const auto jl = std::make_shared<joined_location>(location_future.get());
                        if (jl->location_cbg.empty()) {
                            spdlog::warn("Cannot find CBG for safegraph place: {}", row.safegraph_place_id);
                            const std::vector<v2> f;
                            return hpx::make_ready_future<std::vector<v2>>(f);
                        } else {
                            return hpx::make_ready_future<std::vector<v2>>(handle_row(row, jl));
                        }

                    }).get();
        };
        [[nodiscard]] std::vector<v2> handle_row(const weekly_pattern &row, const std::shared_ptr<joined_location> &jl) const {
            const auto visits = extract_cbg_visits(row);
            auto centroid_future = get_centroid_map(visits);
            std::vector<v2> row_expanded = expandRow(row, visits);
            return detail::compute_distance(jl, row_expanded, centroid_future);
        };
        [[nodiscard]] absl::flat_hash_map<std::string, OGRPoint>
        get_centroid_map(const std::vector<std::pair<std::string, std::uint16_t>> &visits) const {
            std::vector<std::string> cbgs;
            std::transform(visits.begin(), visits.end(), std::back_inserter(cbgs), [](const auto &pair) {
                return pair.first;
            });

            const auto centroids = _s->get_centroids(cbgs).get();

            return absl::flat_hash_map<std::string, OGRPoint>(centroids.begin(), centroids.end());
        }

    protected:
        std::unique_ptr<components::JoinedLocation> _l;
        std::unique_ptr<components::CBGShapefileWrapper> _s;
        components::TileConfiguration _tc;
        date::sys_days _begin_date;
        date::sys_days _end_date;

    };
}

#endif //MOBILITY_CPP_BASESAFEGRAPHMAPPER_HPP