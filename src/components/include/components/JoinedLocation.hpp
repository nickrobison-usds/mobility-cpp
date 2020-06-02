//
// Created by Nicholas Robison on 5/22/20.
//

#ifndef MOBILITY_CPP_JOINEDLOCATION_HPP
#define MOBILITY_CPP_JOINEDLOCATION_HPP

#include "../../server/JoinedLocationServer.hpp"
#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

namespace components {


    class JoinedLocation
            : public hpx::components::client_base<JoinedLocation, components::server::JoinedLocationServer> {

    public:
        explicit JoinedLocation(hpx::future<hpx::id_type> &&f) : client_base(std::move(f)) {};

        explicit JoinedLocation(hpx::id_type &&f) : client_base(std::move(f)) {};

        [[nodiscard]] hpx::future<std::vector<safegraph_location>> invoke();

        hpx::future<joined_location> find_location(const std::string &safegraph_place_id) const;
    };
}

#endif //MOBILITY_CPP_JOINEDLOCATION_HPP
