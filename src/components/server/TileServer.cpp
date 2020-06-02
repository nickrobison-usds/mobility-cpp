//
// Created by Nicholas Robison on 6/1/20.
//

#include "TileServer.hpp"
#include <absl/strings/str_split.h>
#include <components/JoinedLocation.hpp>
#include <io/csv_reader.hpp>
#include "spdlog/spdlog.h"

#include <algorithm>

GDALDatasetUniquePtr openShapefile(const std::string &shapefile_name) {
    // GDAL Features don't support multi-threaded queries, so we open the dataset on each thread, to work around this.
    // A future improvement should be to cache this in the thread itself.
    io::Shapefile s(shapefile_name);
    return s.openFile();
}

namespace components::server {

    TileServer::TileServer() : _dim(), _visits(), _distances(), _p(openShapefile(std::string("hello"))) {
        // Not used
    };

    void TileServer::init(const std::string &filename, const components::TileDimension &dim,
                          std::size_t num_nodes) {
        _dim = dim;

        // Create and fill the matrices
        _distances.reserve(_dim._time_count);
        _visits.reserve(_dim._time_count);

        for (int i = 0; i < 10; i++) {
            _distances.emplace_back(_dim._cbg_count, _dim._cbg_count, 0.0F);
            _visits.emplace_back(_dim._cbg_count, _dim._cbg_count, 0.0F);
        }

        // Read the CSV and filter out any that don't fit in
        const auto rows = extract_rows(filename);

        // iterate through each of the rows, figure out its CBG and expand it.
        JoinedLocation l(hpx::find_here());

        for (const auto &row : rows) {
            const auto loc_future = l.find_location(row.safegraph_place_id);
            const auto row_future = hpx::async(expandRow, row);
        }

    }

    std::vector<weekly_pattern> TileServer::extract_rows(const string &filename) {

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
        uint16_t visits_by_day;
        uint16_t visits_by_each_hour;
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
                   const uint16_t visits_by_day,
                   const uint16_t visits_by_each_hour,
                   const uint64_t poi_cbg,
                   const string &visitor_home_cbgs) {
                    weekly_pattern loc{safegraph_place_id,
                                       location_name,
                                       date_range_start,
                                       date_range_end,
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

    std::tuple<std::uint64_t, std::uint64_t, double> TileServer::computeDistance(const safegraph_location &row) const {

        OGRPoint loc(row.longitude, row.latitude);

        const auto layer = _p->GetLayer(0);
        // Set a new filter on the shapefile layer
        layer->SetSpatialFilter(&loc);
        // Find the cbg that intersects (which should be the first one)
        spdlog::debug("Matching features: {}", layer->GetFeatureCount(true));
        const auto feature = layer->GetNextFeature();
        const auto loc_cbg_str = feature->GetFieldAsString("GEOID");
        const auto geom = feature->GetGeometryRef();
        OGRPoint cbg_centroid;
        geom->Centroid(&cbg_centroid);
        const double distance = cbg_centroid.Distance(&loc);

        // Parse the strings to longs
        std::string::size_type sz;
        const std::uint64_t loc_cbg = std::stol(loc_cbg_str, &sz);
        // This is completely wrong
        const std::uint64_t dest_cbg = std::stol(row.safegraph_place_id, &sz);

        return std::tuple<std::uint64_t, std::uint64_t, double>(loc_cbg, dest_cbg, distance);
    }

    std::vector<v2> TileServer::expandRow(const weekly_pattern &row) {

        const auto split_visits = absl::StrSplit(row.visitor_home_cbgs, ",");

        const std::vector<int> visits;

//        std::transform(split_visits.begin(), split_visits.end(), visits.begin(), [](const auto &tr) {
//            int val;
//            std::from_chars(tr.data(), tr.data() + tr.size(), val);
//            return val;
//        });


        return {};
    }
}
