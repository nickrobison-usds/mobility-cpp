//
// Created by Nicholas Robison on 9/14/20.
//

#include "components/BaseSafegraphMapper.hpp"
#include <shared/QuotedLineSplitter.hpp>
#include <absl/strings/str_split.h>
#include <boost/regex.hpp>

static const boost::regex brackets(R"(\[|\]|")");
static const boost::regex cbg_map_replace("{|\"|}");

namespace components {

    size_t compute_temporal_offset(const date::sys_days &start_date, const date::sys_days &row_date) {
        const auto diff = row_date - start_date;
        return std::abs(diff.count());
    }

    std::vector<v2>
    expandRow(const weekly_pattern &row,
              const std::vector<std::pair<std::string, std::uint16_t>> &cbg_visits) {
        // Extract the number of visits each day
        const auto replaced = boost::regex_replace(row.visits_by_day, brackets, std::string(""));
        // Return a vector of string, because stoi doesn't support string_view.
        const std::vector<std::string> split_visits = absl::StrSplit(replaced, ',');
        std::vector<std::uint16_t> visits;
        std::transform(split_visits.begin(), split_visits.end(), std::back_inserter(visits), [](const auto &tr) {
            std::uint16_t v = 0;
            try {
                v = std::stoi(tr);
            } catch (std::invalid_argument &e) {
                spdlog::error("No conversion for {}", tr);
            }
            return v;
        });

        std::vector<v2> output;
        const std::size_t r = visits.size() * cbg_visits.size();
        spdlog::debug("Expecting {} rows", r);
        output.reserve(r);

        // Iterate through both sets and generate a v2 struct for each day/cbg pair
        for (int i = 0; i < visits.size(); i++) {
            const auto visit = visits[i];
            std::for_each(cbg_visits.begin(), cbg_visits.end(), [&row, &output, &visit, i](const auto &cbg_pair) {
                const v2 day{row.safegraph_place_id, row.date_range_start + date::days{i}, "", cbg_pair.first, visit,
                             0.0F, 0.0F};
                output.push_back(day);
            });
        }
        return output;
    }

    std::vector<std::pair<std::string, std::uint16_t>> extract_cbg_visits(const weekly_pattern &row) {
        // Extract the CBGs which get visited
        const auto cbg_replaced = boost::regex_replace(row.visitor_home_cbgs, cbg_map_replace, "");
        // Split into key/pairs, then split the pairs
        const auto visit_pairs = absl::StrSplit(cbg_replaced, ',');
        std::vector<std::pair<std::string, std::uint16_t>> cbg_visits;
        std::transform(visit_pairs.begin(), visit_pairs.end(), std::back_inserter(cbg_visits), [](const auto &kv_pair) {
            const std::pair<std::string, std::string> split_pair = absl::StrSplit(kv_pair, ':');
            std::uint16_t v = 0;
            // If the value is empty, then we don't even need to try for the stoi. And we avoid throwing/catching the exception
            if (!split_pair.second.empty()) {
                try {
                    v = std::stoi(split_pair.second);
                } catch (std::invalid_argument &e) {
                    spdlog::error("Cannot convert {} for {}", split_pair.second, split_pair.first);
                }
            }
            return std::make_pair(split_pair.first, v);
        });

        std::vector<std::pair<std::string, std::uint16_t>> filtered;
        std::copy_if(cbg_visits.begin(), cbg_visits.end(), std::back_inserter(filtered), [](const auto &v) {
            return !v.first.empty();
        });

        return filtered;
    }

    template<class Backend, class Output>
    absl::flat_hash_map<std::string, OGRPoint>
    BaseSafegraphMapper<Backend, Output>::get_centroid_map(const std::vector<std::pair<std::string, std::uint16_t>> &visits) const {
        std::vector<std::string> cbgs;
        std::transform(visits.begin(), visits.end(), std::back_inserter(cbgs), [](const auto &pair) {
            return pair.first;
        });

        const auto centroids = _s->get_centroids(cbgs).get();

        return absl::flat_hash_map<std::string, OGRPoint>(centroids.begin(), centroids.end());
    }

    template<class Backend, class Output>
    weekly_pattern BaseSafegraphMapper<Backend, Output>::parse_string(const std::string_view v) {
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
                shared::ConversionUtils::convert_empty<uint64_t>(splits[15]),
                splits[16]
        };

        return w;
    }

    template<class Backend, class Output>
    std::vector<v2> BaseSafegraphMapper<Backend, Output>::process_row(const weekly_pattern &row) const {
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
    }

    template<class Backend, class Output>
    std::vector<v2>
    BaseSafegraphMapper<Backend, Output>::handle_row(const weekly_pattern &row, const std::shared_ptr<joined_location> &jl) const {
        const auto visits = extract_cbg_visits(row);
        auto centroid_future = get_centroid_map(visits);
        std::vector<v2> row_expanded = expandRow(row, visits);
        return compute_distance(jl, row_expanded, centroid_future);
    }
}

