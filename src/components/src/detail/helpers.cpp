//
// Created by Nicholas Robison on 9/16/20.
//

#include "components/detail/helpers.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <nlohmann/json.hpp>
#include <shared/constants.hpp>
#include <shared/debug.hpp>
#include <shared/ConversionUtils.hpp>
#include <shared/DateUtils.hpp>
#include <shared/QuotedLineSplitter.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <string>

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

    std::map<std::string, std::uint16_t> dwell_time_to_map(std::string_view v) {
        // Strip off the leading and trailing `"` characters.
        auto v2 = std::string(v).substr(1, v.size() - 2);
        // Remove the double quoting from the keys
        boost::replace_all(v2, R"("")", R"(")");
        return nlohmann::json::parse(v2);
    }

    weekly_pattern parse_string(const std::string_view v) {
        const auto splits = shared::QuotedStringSplitter(v);

        weekly_pattern w{
                splits[0],
                splits[1],
                shared::DateUtils::to_days(splits[9]),
                shared::DateUtils::to_days(splits[10]),
                shared::ConversionUtils::convert_empty<uint32_t>(splits[11]),
                shared::ConversionUtils::convert_empty<uint32_t>(splits[12]),
                splits[13],
                splits[14],
                splits[15],
                splits[16],
                shared::ConversionUtils::convert_empty<float>(splits[20]),
                dwell_time_to_map(splits[21]),
        };

        return w;
    }
}