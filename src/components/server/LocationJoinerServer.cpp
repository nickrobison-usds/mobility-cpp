//
// Created by Nicholas Robison on 5/22/20.
//
#include "LocationJoinerServer.hpp"
#include <hpx/parallel/executors.hpp>

namespace components::server {
    void LocationJoinerServer::invoke() const {
        // Read the shapefile and one of the CSV files.
    }

    hpx::future<std::vector<std::string>> LocationJoinerServer::read_shapefile() {
        // Get a reference to one of the IO specific HPX io_service objects ...
        hpx::parallel::execution::io_pool_executor executor;

        // Create a new shapefile reader
//        io::Shapefile s(_shapefile);

//        s.read<census_cbg>([](const OGRFeatureUniquePtr &s) {
//            s->GetFieldCount();
//            census_cbg c;
//            return c;
//        });


        return hpx::future<std::vector<std::string>>();
    }
}
