//
// Created by Nicholas Robison on 6/2/20.
//

#include "components/TileClient.hpp"
#include "server/ShapefileServer.hpp"


namespace components {

    TileClient::TileClient(const std::string &output_dir, const std::string &output_name): client_base(hpx::new_<server::TileServer>(hpx::find_here(), output_dir, output_name)) {
        // Not used
    };

    hpx::future<void>
    TileClient::init(const TileConfiguration &dim, std::size_t num_nodes) const {
        return hpx::async<server::TileServer::init_action>(get_id(), dim, num_nodes);
    }
}

typedef hpx::components::component<
        components::server::TileServer
> tile_client_type;

HPX_REGISTER_COMPONENT(tile_client_type, TileClient);
HPX_REGISTER_ACTION(::components::server::TileServer::init_action);