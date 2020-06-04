//
// Created by Nicholas Robison on 6/1/20.
//

#include "TileServer.hpp"
#include <absl/container/flat_hash_map.h>
#include <absl/strings/str_split.h>
#include <boost/regex.hpp>
#include <components/JoinedLocation.hpp>
#include <components/ShapefileWrapper.hpp>
#include <hpx/parallel/execution.hpp>
#include <hpx/parallel/algorithms/transform.hpp>
#include <io/csv_reader.hpp>
#include "spdlog/spdlog.h"

#include <algorithm>

static const boost::regex brackets("\\[|\\]");
static const boost::regex cbg_map_replace("{|\"|}");

GDALDatasetUniquePtr openShapefile(const std::string &shapefile_name) {
    // GDAL Features don't support multi-threaded queries, so we open the dataset on each thread, to work around this.
    // A future improvement should be to cache this in the thread itself.
    io::Shapefile s(shapefile_name);
    return s.openFile();
}

namespace components::server {

    std::vector<weekly_pattern> extract_rows(const string &filename) {

        // Get date from filename
        spdlog::debug("Reading {}", filename);
        io::CSVLoader<17, true> l(filename);

        string safegraph_place_id;
        string location_name;
        string street_address;
        string city;
        string region;
        string postal_code;
        string iso_country_code;
        string safegraph_brand_ids;
        string brands;
        string date_range_start;
        string date_range_end;
        uint16_t raw_visit_counts;
        uint16_t raw_visitor_counts;
        string visits_by_day;
        string visits_by_each_hour;
        uint64_t poi_cbg;
        string visitor_home_cbgs;

        return l.read<weekly_pattern>(
                [](const string &safegraph_place_id,
                   const string &location_name,
                   const string &street_address,
                   const string &city,
                   const string &region,
                   const string &postal_code,
                   const string &iso_country_code,
                   const string &safegraph_brand_ids,
                   const string &brands,
                   const string &date_range_start,
                   const string &date_range_end,
                   const uint16_t raw_visit_counts,
                   const uint16_t raw_visitor_counts,
                   const string visits_by_day,
                   const string visits_by_each_hour,
                   const uint64_t poi_cbg,
                   const string &visitor_home_cbgs) {

                    // Parse the start/end dates
                    std::istringstream start{date_range_start};
                    std::istringstream end{date_range_end};
                    date::sys_days start_date;
                    date::sys_days end_date;
                    start >> date::parse("%F", start_date);
                    end >> date::parse("%F", end_date);


                    weekly_pattern loc{safegraph_place_id,
                                       location_name,
                                       start_date,
                                       end_date,
                                       raw_visit_counts,
                                       raw_visitor_counts,
                                       visits_by_day,
                                       visits_by_each_hour,
                                       poi_cbg,
                                       visitor_home_cbgs};

                    return loc;
                }, safegraph_place_id,
                location_name,
                street_address,
                city,
                region,
                postal_code,
                iso_country_code,
                safegraph_brand_ids,
                brands,
                date_range_start,
                date_range_end,
                raw_visit_counts,
                raw_visitor_counts,
                visits_by_day,
                visits_by_each_hour,
                poi_cbg,
                visitor_home_cbgs);
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


    std::vector<v2> expandRow(const weekly_pattern &row, const std::vector<std::pair<std::string, std::uint16_t>> &cbg_visits) {
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
            for (const auto& cbg_pair : cbg_visits) {
                const v2 day{row.safegraph_place_id, row.date_range_start + date::days{i}, cbg_pair.first, visit, 0.0F, 0.0F};
                output.push_back(day);
            }
        }
        spdlog::debug("Returning output");
        return output;
    };

    TileServer::TileServer() : _dim(), _visits(), _distances() {
        // Not used
    };

    void TileServer::init(const std::string &filename, const components::TileDimension &dim,
                          std::size_t num_nodes) {
        _dim = dim;

        // Create and fill the matrices
        _distances.reserve(_dim._time_count);
        _visits.reserve(_dim._time_count);

        visit_matrix v(100, 100, 0);
        distance_matrix d(100, 100, 0.0F);

        // This is broken, I think it has to do with the wonky way we're initializing things
        // We shouldn't do it this way.

//        for (int i = 0; i < 10; i++) {
////            visit_matrix v(100, 100, 0);
////            distance_matrix d(100, 100, 0.0F);
//            _visits.emplace_back(100, 100, 0);
////            _distances.push_back(&d);
//        }

        // Read the CSV and filter out any that don't fit in
        const auto rows = extract_rows(filename);

        // iterate through each of the rows, figure out its CBG and expand it.
        spdlog::debug("Initializing location join component");
        JoinedLocation l({}, std::string("Not a shapefile"), std::string("./test-dir/poi.parquet"));
        spdlog::debug("Initializing shapefile component");
        ShapefileWrapper s(std::string("/Users/raac/Development/covid/mobility-analysis/data/reference/census/block_groups.shp"));


        std::vector<hpx::future<std::vector<v2>>> results;
        results.reserve(rows.size());
        for (const auto &row : rows) {
            const auto visits = extract_cbg_visits(row);
            std::vector<std::string> cbgs;
            std::transform(visits.begin(), visits.end(), std::back_inserter(cbgs), [](const auto &pair) {
                return pair.first;
            });

            hpx::future<joined_location> loc_future = l.find_location(row.safegraph_place_id);
            hpx::future<std::vector<v2>> row_future = hpx::async(&expandRow, row, visits);

            using hpx::dataflow;
            using hpx::util::unwrapping;

            auto centroid_future = s.get_centroids(cbgs).then(unwrapping([](const auto &centroids) {
                absl::flat_hash_map<std::string, OGRPoint> map(centroids.begin(), centroids.end());
                return map;
            }));

            auto res = dataflow(unwrapping([this](const joined_location &loc, const std::vector<v2> &patterns, const auto &centroids) {
                spdlog::debug("Calculating distances for {}", loc.safegraph_place_id);
                const OGRPoint loc_point(loc.longitude, loc.latitude);
                patterns.size();
                std::vector<v2> o;
                o.reserve(patterns.size());
                std::transform(patterns.begin(), patterns.end(), std::back_inserter(o), [&centroids, &loc_point](auto &row) {
                    const auto cbg_centroid = centroids.at(row.visit_cbg);
                    const auto distance = loc_point.Distance(&cbg_centroid);
                    v2 r2{row.safegraph_place_id, row.visit_date, row.visit_cbg, row.visits, distance, 0.0F};
                    return r2;
                });
                spdlog::debug("Finished calculating distances for {}", loc.safegraph_place_id);
                return o;
            }), loc_future, row_future, centroid_future);

            results.push_back(std::move(res));
        };
        spdlog::debug("Waiting for {} rows", results.size());
        hpx::wait_all(results);
        spdlog::debug("It's done");
    }
}
