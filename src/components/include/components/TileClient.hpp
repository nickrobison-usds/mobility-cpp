//
// Created by Nicholas Robison on 6/2/20.
//

#ifndef MOBILITY_CPP_TILECLIENT_HPP
#define MOBILITY_CPP_TILECLIENT_HPP

#include "../../src/server/TileServer.hpp"
#include <hpx/hpx.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

namespace components {
class TileClient : public hpx::components::client_base<TileClient, components::server::TileServer> {

    public:
        explicit TileClient(hpx::future<hpx::id_type> &&f) : client_base(std::move(f)) {};

        explicit TileClient(hpx::id_type &&f) : client_base(std::move(f)) {};

        TileClient(const std::string &output_dir, const std::string &output_name, const std::string &cbg_shp, const std::string &poi_parquet);

        hpx::future<void> init(const TileConfiguration &dim, std::size_t num_nodes) const;
    };
}


#endif //MOBILITY_CPP_TILECLIENT_HPP
