//
// Created by Nicholas Robison on 6/9/20.
//

#include "RowProcessor.hpp"
#include <absl/strings/str_split.h>
#include <boost/regex.hpp>
#include "spdlog/spdlog.h"

static const boost::regex brackets("\\[|\\]");
static const boost::regex cbg_map_replace("{|\"|}");

using hpx::dataflow;
using hpx::util::unwrapping;

namespace components {

    size_t compute_temporal_offset(const date::sys_days &start_date, const date::sys_days &row_date) {
        const auto diff = row_date - start_date;
        return diff.count();
    }

    std::vector<v2>
    compute_distance(const std::shared_ptr<joined_location> loc, const std::vector<v2> &patterns,
                     const absl::flat_hash_map<std::string, OGRPoint> &centroids) {
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
        return o;
    }

    std::vector<std::pair<std::string, std::uint16_t>> extract_cbg_visits(const std::shared_ptr<weekly_pattern> row) {
        // Extract the CBGs which get visited
        const auto cbg_replaced = boost::regex_replace(row->visitor_home_cbgs, cbg_map_replace, "");
        // Split into key/pairs, then split the pairs
        const auto visit_pairs = absl::StrSplit(cbg_replaced, ',');
        std::vector<std::pair<std::string, std::uint16_t>> cbg_visits;
        std::transform(visit_pairs.begin(), visit_pairs.end(), std::back_inserter(cbg_visits), [](const auto &kv_pair) {
            const std::pair<std::string, std::string> split_pair = absl::StrSplit(kv_pair, ':');
            std::uint16_t v = 0;
            try {
                v = std::stoi(split_pair.second);
            } catch (std::invalid_argument &e) {
                spdlog::error("Cannot convert {} for {}", split_pair.second, split_pair.first);
            }
            return std::make_pair(split_pair.first, v);
        });

        std::vector<std::pair<std::string, std::uint16_t>> filtered;
        std::copy_if(cbg_visits.begin(), cbg_visits.end(), std::back_inserter(filtered), [](const auto &v) {
            return !v.first.empty();
        });

        return filtered;
    };

    std::vector<v2>
    expandRow(const std::shared_ptr<weekly_pattern> row,
              const std::shared_ptr<std::vector<std::pair<std::string, std::uint16_t>>> cbg_visits) {
        // Extract the number of visits each day
        const auto replaced = boost::regex_replace(row->visits_by_day, brackets, std::string(""));
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
        const std::size_t r = visits.size() * cbg_visits->size();
        spdlog::debug("Expecting {} rows", r);
        output.reserve(r);

        // Iterate through both sets and generate a v2 struct for each day/cbg pair
        for (int i = 0; i < visits.size(); i++) {
            const auto visit = visits[i];
            std::for_each(cbg_visits->begin(), cbg_visits->end(), [&row, &output, &visit, i](const auto &cbg_pair) {
                const v2 day{row->safegraph_place_id, row->date_range_start + date::days{i}, "", cbg_pair.first, visit,
                             0.0F, 0.0F};
                output.push_back(day);
            });
        }
        return output;
    };

    hpx::future<void> RowProcessor::process_row(const std::shared_ptr<weekly_pattern> row) {
        return _l.find_location(row->safegraph_place_id).then(
                [this, row](hpx::future<joined_location> location_future) {
                    const auto jl = std::make_shared<joined_location>(location_future.get());
                    const auto offset = calculate_cbg_offset(jl->location_cbg);
//                     If the cbg is outside of our partition, then return immediately.
//                     Otherwise, start the processing
                    if (offset < _conf._cbg_min || offset >= _conf._cbg_max) {
                        spdlog::debug("CBG {} is outside of boundary {}/{}", offset, _conf._cbg_min, _conf._cbg_max);
                        return hpx::make_ready_future();
                    } else {
                        return handle_row(row, jl);
                    }
                });
    };

    hpx::future<void>
    RowProcessor::handle_row(const std::shared_ptr<weekly_pattern> row, const std::shared_ptr<joined_location> jl) {
        const auto visits = std::make_shared<std::vector<std::pair<std::string, std::uint16_t>>>(
                extract_cbg_visits(row));
        auto centroid_future = get_centroid_map(visits);
        hpx::future<std::vector<v2>> row_future = hpx::async(&expandRow, row, visits);

        auto distance_res = dataflow(unwrapping(&compute_distance), hpx::make_ready_future(jl), row_future,
                                     centroid_future);

        // Not sure why I can't just call the member function directly, but this seems ok
        return distance_res.then([this](hpx::future<std::vector<v2>> distance_future) {
            return insert_rows(std::move(distance_future));
        });
    };

    hpx::future<absl::flat_hash_map<std::string, OGRPoint>>
    RowProcessor::get_centroid_map(const std::shared_ptr<std::vector<std::pair<std::string, std::uint16_t>>> visits) {
        auto cbg_future = hpx::async([visits]() {
            std::vector<std::string> cbgs;
            std::transform(visits->begin(), visits->end(), std::back_inserter(cbgs), [](const auto &pair) {
                return pair.first;
            });
            return cbgs;
        });

        return cbg_future.then([this](auto f) {
            const auto cbgs = f.get();
            return _s.get_centroids(cbgs).then(unwrapping([](const auto &centroids) {
                absl::flat_hash_map<std::string, OGRPoint> map(centroids.begin(), centroids.end());
                return map;
            }));
        });
    };

    hpx::future<void> RowProcessor::insert_rows(hpx::future<std::vector<v2>> rows) {
        const auto expanded_rows = rows.get();

        if (expanded_rows.empty()) {
            spdlog::debug("No rows to add, skipping insert");
            return hpx::make_ready_future();
        }
        spdlog::debug("Adding {} rows to matrices", expanded_rows.size());
        std::for_each(expanded_rows.begin(), expanded_rows.end(),
                      [this](const v2 &expanded_row) {
                          // Compute the temporal offset
                          const auto t_offset = compute_temporal_offset(_start_date,
                                                                        expanded_row.visit_date);
                          _matricies.insert(t_offset,
                                            calculate_cbg_offset(expanded_row.location_cbg),
                                            calculate_cbg_offset(expanded_row.visit_cbg),
                                            expanded_row.visits, expanded_row.distance);
                      });
        spdlog::debug("Finished adding rows");
        return hpx::make_ready_future();
    }

    /**
     * Compute the local offset for a given CBG code
     * @param map - Map of CBG-codes to their size_t matrix offset
     * @param cbg_code - CBG code (string)
     * @return offset
     */
    size_t RowProcessor::calculate_cbg_offset(const std::string &cbg_code) const {
        return _offset_map.left.at(cbg_code);
    };
}