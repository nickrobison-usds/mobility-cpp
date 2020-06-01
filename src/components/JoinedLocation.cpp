//
// Created by Nicholas Robison on 5/22/20.
//

#include "components/JoinedLocation.hpp"

namespace components {

    hpx::future<std::vector<safegraph_location>> JoinedLocation::invoke() const {
        return hpx::async<server::JoinedLocationServer::invoke_action>(get_id());
    }
}
typedef hpx::components::component<
        components::server::JoinedLocationServer
> location_joiner_type;

HPX_REGISTER_COMPONENT(location_joiner_type, LocationJoiner);
HPX_REGISTER_ACTION(::components::server::JoinedLocationServer::invoke_action);
