//
// Created by Nicholas Robison on 5/22/20.
//

#include "components/LocationJoiner.hpp"

namespace components {

    hpx::future<std::vector<safegraph_location>> LocationJoiner::invoke() const {
        return hpx::async<server::LocationJoinerServer::invoke_action>(get_id());
    }
}
typedef hpx::components::component<
        components::server::LocationJoinerServer
> location_joiner_type;

HPX_REGISTER_COMPONENT(location_joiner_type, LocationJoiner);
HPX_REGISTER_ACTION(::components::server::LocationJoinerServer::invoke_action);
