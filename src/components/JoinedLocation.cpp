//
// Created by Nicholas Robison on 5/22/20.
//

#include "components/JoinedLocation.hpp"

namespace components {

    hpx::future<std::vector<safegraph_location>> JoinedLocation::invoke() {
        return hpx::async<server::JoinedLocationServer::invoke_action>(get_id());
    }

    hpx::future<joined_location> JoinedLocation::find_location(const std::string safegraph_place_id) const {
        return hpx::async<server::JoinedLocationServer::find_location_action>(get_id(), safegraph_place_id);
    }

    JoinedLocation::JoinedLocation(std::vector<std::string> csv_files, std::string shapefile, std::string parquet_file): client_base(hpx::new_<server::JoinedLocationServer>(hpx::find_here(), csv_files, shapefile, parquet_file))  {
    }
}
typedef hpx::components::component<
        components::server::JoinedLocationServer
> location_joiner_type;

HPX_REGISTER_COMPONENT(location_joiner_type, LocationJoiner);
HPX_REGISTER_ACTION(::components::server::JoinedLocationServer::invoke_action);
HPX_REGISTER_ACTION(::components::server::JoinedLocationServer::find_location_action);
