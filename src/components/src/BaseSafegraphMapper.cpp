//
// Created by Nicholas Robison on 9/14/20.
//

#include "components/BaseSafegraphMapper.hpp"
#include <absl/strings/str_split.h>
#include <boost/regex.hpp>

static const boost::regex brackets(R"(\[|\]|")");
static const boost::regex cbg_map_replace("{|\"|}");

namespace components {

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
}