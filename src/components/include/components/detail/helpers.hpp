//
// Created by Nicholas Robison on 9/16/20.
//

#ifndef MOBILITY_CPP_HELPERS_HPP
#define MOBILITY_CPP_HELPERS_HPP

#include <absl/container/flat_hash_map.h>
#include <date/date.h>
#include <shared/data.hpp>
#include <string_view>
#include <utility>
#include <vector>

namespace components::detail {
    size_t compute_temporal_offset(const date::sys_days &start_date, const date::sys_days &row_date);

    std::vector<v2>
    compute_distance(std::shared_ptr<joined_location> loc, const std::vector<v2> &patterns,
                     const absl::flat_hash_map<std::string, OGRPoint> &centroids);

    weekly_pattern parse_string(std::string_view v);
}

#endif //MOBILITY_CPP_HELPERS_HPP
