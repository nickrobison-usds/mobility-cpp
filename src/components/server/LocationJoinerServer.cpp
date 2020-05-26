//
// Created by Nicholas Robison on 5/22/20.
//
#include "LocationJoinerServer.hpp"
#include <io/csv_reader.hpp>
#include <hpx/parallel/executors.hpp>
#include <hpx/parallel/algorithms/transform.hpp>

#include "spdlog/spdlog.h"

namespace par = hpx::parallel;

namespace components::server {

    std::vector<safegraph_location> LocationJoinerServer::invoke() const {

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
//        string street_address;
//        string city;
//        string region;
//        uint32_t postal_code;
//        string iso_country_code;
//        string phone_number;
//        string open_hours;
//        string category_tags;

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
                                               OGRPoint(longitude, latitude)};

                        return loc;
                    }, safegraph_place_id, parent_safegraph_place_id, location_name, safegraph_brand_ids, brands,
                    top_category, sub_category, naics_code, latitude,
                    longitude); //, street_address, city, region, postal_code, iso_country_code, phone_number, open_hours, category_tags);
            output.reserve(output.size() + outs.size());
            std::move(outs.begin(), outs.end(), back_inserter(output));
        }

        spdlog::debug("Finishing reading files with {} locations", output.size());

        std::string file_capture = _shapefile;

        par::transform(par::execution::par_unseq, output.begin(), output.end(), output.begin(), [&file_capture](safegraph_location &loc) {
            // GDAL Features don't support multi-threaded queries, so we open the dataset on each thread, to work around this.
            // A future improvement should be to cache this in the thread itself.
            io::Shapefile s(file_capture);
            GDALDatasetUniquePtr p = s.openFile();
            const auto layer = p->GetLayer(0);
            // Set a new filter on the shapefile layer
            layer->SetSpatialFilter(&loc.location);
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

    hpx::future<int> LocationJoinerServer::read_shapefile(std::shared_ptr<GDALDataset> &ptr) const {
        // Get a reference to one of the IO specific HPX io_service objects ...
        hpx::parallel::execution::io_pool_executor executor;

        // Create a new shapefile reader

//        ptr = std::move(p);
        return hpx::make_ready_future<int>(1);
//        return std::move(p);
    }

    hpx::future<std::vector<safegraph_location>> LocationJoinerServer::read_csv(std::string csv_file) const {


        return hpx::make_ready_future<std::vector<safegraph_location>>();
    }


}
