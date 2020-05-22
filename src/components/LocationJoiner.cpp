//
// Created by Nicholas Robison on 5/22/20.
//

#include "LocationJoiner.hpp"

namespace components {


    LocationJoiner::LocationJoiner(hpx::future<hpx::id_type> &&f) : client_base(std::move(f)) {}

    LocationJoiner::LocationJoiner(hpx::id_type &&f) : client_base(f) {}

    LocationJoiner::LocationJoiner(): client_base(hpx::new_<server::LocationJoinerServer>(hpx::find_here())) {}

    hpx::future<void> LocationJoiner::invoke() const {
        return hpx::async<server::LocationJoinerServer::invoke_action>(get_id());
    }

}
