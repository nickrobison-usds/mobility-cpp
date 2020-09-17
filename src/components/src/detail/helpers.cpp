//
// Created by Nicholas Robison on 9/16/20.
//

#include "components/detail/helpers.hpp"
#include <shared/constants.hpp>
#include <shared/debug.hpp>
#include <spdlog/spdlog.h>

namespace components::detail {

    size_t compute_temporal_offset(const date::sys_days &start_date, const date::sys_days &row_date) {
        const auto diff = row_date - start_date;
        return std::abs(diff.count());
    }

    std::vector<v2>
    compute_distance(const std::shared_ptr<joined_location> loc, const std::vector<v2> &patterns,
                     const absl::flat_hash_map<std::string, OGRPoint> &centroids) {
        const auto dp = shared::DebugInterval::create_debug_point(shared::SignPostCode::COMPUTE_DISTANCES);
        spdlog::debug("Calculating distances for {}", loc->safegraph_place_id);
        const OGRPoint loc_point(loc->longitude, loc->latitude);
        patterns.size();
        std::vector<v2> o;
        o.reserve(patterns.size());
        std::transform(patterns.begin(), patterns.end(), std::back_inserter(o),
                       [&centroids, &loc_point, &loc](auto &row) {
                           const auto cbg_centroid = centroids.at(row.visit_cbg);
                           const auto distance = loc_point.Distance(&cbg_centroid);
                           v2 r2{row.safegraph_place_id, row.visit_date, loc->location_cbg,
                                 row.visit_cbg, row.visits, distance, 0.0F};
                           return r2;
                       });
        spdlog::debug("Finished calculating distances for {}", loc->safegraph_place_id);
        dp.stop();
        return o;
    }
}