//
// Created by Nicholas Robison on 6/3/20.
//

#include "components/ShapefileWrapper.hpp"

namespace components {

    ShapefileWrapper::ShapefileWrapper(std::string shapefile): client_base(hpx::new_<server::ShapefileServer>(hpx::find_here(), shapefile)) {

    };

    hpx::future<std::vector<std::pair<std::string, OGRPoint>>> ShapefileWrapper::get_centroids(const std::vector<std::string> &geoids) const {
        return hpx::async<server::ShapefileServer::get_centroids_action>(get_id(), geoids);
    }

    hpx::future<server::ShapefileServer::offset_type> ShapefileWrapper::build_offsets() const {
        return hpx::async<server::ShapefileServer::build_offsets_action>(get_id());
    }
}

typedef hpx::components::component<
        components::server::ShapefileServer
> shapefile_type;

HPX_REGISTER_COMPONENT(shapefile_type, ShapefileWrapper);
HPX_REGISTER_ACTION(::components::server::ShapefileServer::get_centroids_action);
HPX_REGISTER_ACTION(::components::server::ShapefileServer::build_offsets_action);