//
// Created by Nicholas Robison on 5/22/20.
//

#ifndef MOBILITY_CPP_LOCATIONJOINER_HPP
#define MOBILITY_CPP_LOCATIONJOINER_HPP

#include "server/LocationJoinerServer.hpp"
#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>

namespace components {


    class LocationJoiner
            : public hpx::components::client_base<LocationJoiner, components::server::LocationJoinerServer> {

    public:
        explicit LocationJoiner(hpx::future<hpx::id_type> &&f);

        explicit LocationJoiner(hpx::id_type &&f);

        explicit LocationJoiner();

        hpx::future<void> invoke() const;
    };
}

#endif //MOBILITY_CPP_LOCATIONJOINER_HPP
