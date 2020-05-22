//
// Created by Nicholas Robison on 5/22/20.
//

#ifndef MOBILITY_CPP_LOCATIONJOINERSERVER_HPP
#define MOBILITY_CPP_LOCATIONJOINERSERVER_HPP

#include <string>
#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

namespace components::server {


    class LocationJoinerServer : public hpx::components::component_base<LocationJoinerServer> {

    public:
        void invoke() const;

        HPX_DEFINE_COMPONENT_ACTION(LocationJoinerServer, invoke);
    };
}


HPX_REGISTER_ACTION_DECLARATION(::components::server::LocationJoinerServer::invoke_action, location_joiner_invoke_action);

#endif //MOBILITY_CPP_LOCATIONJOINERSERVER_HPP
