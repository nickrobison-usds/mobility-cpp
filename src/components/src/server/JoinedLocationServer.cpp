//
// Created by Nicholas Robison on 5/22/20.
//
#include "JoinedLocationServer.hpp"
#include <boost/filesystem.hpp>
#include <io/csv_reader.hpp>
#include <io/parquet.hpp>
#include <hpx/parallel/executors.hpp>
#include <hpx/parallel/algorithms/transform.hpp>
#include <algorithm>
#include <utility>

#include "spdlog/spdlog.h"

namespace par = hpx::parallel;
namespace fs = boost::filesystem;

namespace components::server {

    std::vector<std::string> getParquetFiles(const std::string &path_string) {
        // Determine if the file path is a directory or a single file. If it's a directory, we'll need to iterate over the values
        const fs::path file_path(path_string);
        std::vector<std::string> files;
        if (fs::is_regular_file(file_path)) {
            files.push_back(file_path.string());
        } else {
            for (auto &p : fs::directory_iterator(file_path)) {
                // Skip if not a file or not parquet
                if (!boost::filesystem::is_regular_file(p.status()))
                    continue;

                if (fs::extension(p) != ".parquet") {
                    continue;
                }

                files.push_back(p.path().string());
            };
        }

        return files;
    }

    JoinedLocationServer::JoinedLocationServer(std::vector<std::string> csv_files, std::string shapefile,
                                               std::string parquet_file) :
            _csv_file(std::move(csv_files)),
            _shapefile(std::move(shapefile)),
            _parquet(std::move(parquet_file)),
            _cache(loadLocationCache()) {
        parquet_file.length();
    }

    absl::flat_hash_map<std::string, joined_location> JoinedLocationServer::loadLocationCache() const {
        spdlog::debug("Loading POI cache with {}", _shapefile);
        absl::flat_hash_map<std::string, joined_location> m;

        const auto files = getParquetFiles(_shapefile);

        std::for_each(files.begin(), files.end(), [&m](const auto &file) {

            // No idea why we can't get the string to pass correctly.
            const io::Parquet parquet(file);

            const auto table = parquet.read();

            auto latitude = static_pointer_cast<arrow::DoubleArray>(table->column(0)->chunk(0));
            auto longitude = static_pointer_cast<arrow::DoubleArray>(table->column(1)->chunk(0));
            auto location_cbg = static_pointer_cast<arrow::StringArray>(table->column(2)->chunk(0));
            auto location_id = static_pointer_cast<arrow::StringArray>(table->column(3)->chunk(0));

            for (std::int64_t i = 0; i < table->num_rows(); i++) {
                const double lat = latitude->Value(i);
                const double lon = longitude->Value(i);
                const std::string cbg = location_cbg->GetString(i);
                const std::string id = location_id->GetString(i);

                joined_location l = {id, lat, lon, cbg};
                m.emplace(std::make_pair(id, l));
            };
        });
        spdlog::debug("Cache loading complete. {} Entries", m.size());
        return m;
    }

    joined_location JoinedLocationServer::find_location(const std::string &safegraph_place_id) {
        return _cache.at(safegraph_place_id);
    }

    std::vector<safegraph_location> JoinedLocationServer::invoke() {

        std::shared_ptr<GDALDataset> d;
//         Read the shapefile and one of the CSV files.


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

        // Open and read all the CSV files
        std::vector<safegraph_location> output;
        for (const auto &file : _csv_file) {
            spdlog::debug("Reading {}", file);
            io::CSVLoader<10, true> l(file);

            vector<safegraph_location> outs = l.read<safegraph_location>(
                    [](string const &safegraph_place_id, string const &parent_safegraph_place_id,
                       string const &location_name, string const &safegraph_brand_ids, string const &brands,
                       string const &top_category, string const &sub_category, string const &naics_code,
                       double const &latitude, double const &longitude) { //, string const &street_address,
                        // string const &city, string const &region, uint32_t const &postal_code, const string& iso_country_code,
                        //string const &phone_number, string const &open_hours, string const &category_tags) {
                        safegraph_location loc{safegraph_place_id,
                                               parent_safegraph_place_id,
                                               location_name,
                                               safegraph_brand_ids,
                                               brands,
                                               top_category,
                                               sub_category,
                                               naics_code,
                                               latitude,
                                               longitude};

                        return loc;
                    }, safegraph_place_id, parent_safegraph_place_id, location_name, safegraph_brand_ids, brands,
                    top_category, sub_category, naics_code, latitude,
                    longitude); //, street_address, city, region, postal_code, iso_country_code, phone_number, open_hours, category_tags);
            output.reserve(output.size() + outs.size());
            std::move(outs.begin(), outs.end(), back_inserter(output));
        }

        spdlog::debug("Finishing reading files with {} locations", output.size());

        std::string file_capture = _shapefile;

        par::transform(par::execution::par_unseq, output.begin(), output.end(), output.begin(),
                       [&file_capture](safegraph_location &loc) {
                           // GDAL Features don't support multi-threaded queries, so we open the dataset on each thread, to work around this.
                           // A future improvement should be to cache this in the thread itself.
                           io::Shapefile s(file_capture);
                           GDALDatasetUniquePtr p = s.openFile();
                           const auto layer = p->GetLayer(0);
                           // Set a new filter on the shapefile layer
                           OGRPoint location(loc.longitude, loc.latitude);
                           layer->SetSpatialFilter(&location);
                           // Find the cbg that intersects (which should be the first one)
                           spdlog::debug("Matching features: {}", layer->GetFeatureCount(true));
                           const auto feature = layer->GetNextFeature();
                           loc.cbg = feature->GetFieldAsString("GEOID");

                           return loc;
                       });

        return output;

//        std::vector<safegraph_location> g = LocationJoinerServer::read_csv(string("hello")).wait();
//        g.size();


//        hpx::dataflow(
//                hpx::util::unwrapping([d](int v, std::vector<safegraph_location> locations) {
//                    locations.size();
//                }), LocationJoinerServer::read_shapefile(d), );
    }

    hpx::future<int> JoinedLocationServer::read_shapefile(std::shared_ptr<GDALDataset> &ptr) const {
        // Get a reference to one of the IO specific HPX io_service objects ...
        hpx::parallel::execution::io_pool_executor executor;

        // Create a new shapefile reader

//        ptr = std::move(p);
        return hpx::make_ready_future<int>(1);
//        return std::move(p);
    }

    hpx::future<std::vector<safegraph_location>> JoinedLocationServer::read_csv(std::string csv_file) const {


        return hpx::make_ready_future<std::vector<safegraph_location>>();
    }
}
