//
// Created by Nicholas Robison on 6/1/20.
//

#include "TileServer.hpp"
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

    TileServer::TileServer(): _dim(), _visits(), _distances(), _p(openShapefile(std::string("hello"))) {
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

    }

    std::vector<sl2> TileServer::extract_rows(const string &filename) const {

        // Get date from filename
        spdlog::debug("Reading {}", filename);
        io::CSVLoader<10, true> l(filename);

        // Initialize variables for holding the column values
        string safegraph_place_id;
        string parent_safegraph_place_id;
        string location_name;
        string safegraph_brand_ids;
        string brands;
        string top_category;
        string sub_category;
        string naics_code;
        double latitude;
        double longitude;

        return l.read<sl2>(
                [](string const &safegraph_place_id, string const &parent_safegraph_place_id,
                   string const &location_name, string const &safegraph_brand_ids, string const &brands,
                   string const &top_category, string const &sub_category, string const &naics_code,
                   double const &latitude, double const &longitude) {
                    sl2 loc{safegraph_place_id,
                                           parent_safegraph_place_id,
                                           location_name,
                                           safegraph_brand_ids,
                                           brands,
                                           top_category,
                                           sub_category,
                                           naics_code,
                                           0L,
                                           latitude,
                                           longitude};

                    return loc;
                }, safegraph_place_id, parent_safegraph_place_id, location_name, safegraph_brand_ids, brands,
                top_category, sub_category, naics_code, latitude,
                longitude);
    }

    std::tuple<std::uint64_t, std::uint64_t, double> TileServer::computeDistance(const sl2 &row) const {

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

    std::vector<std::uint16_t> TileServer::expandRow(const sl2 &row) const {
        return {};
    }
}
