//
// Created by Nicholas Robison on 6/3/20.
//

#include "components/CBGShapefileWrapper.hpp"

namespace components {

    CBGShapefileWrapper::CBGShapefileWrapper(const std::string& shapefile): client_base(hpx::new_<server::CBGShapefileServer>(hpx::find_here(), shapefile)) {

    };

    hpx::future<std::vector<std::pair<std::string, OGRPoint>>> CBGShapefileWrapper::get_centroids(const std::vector<std::string> &geoids) const {
        return hpx::async<server::CBGShapefileServer::get_centroids_action>(get_id(), geoids);
    }

    hpx::future<server::CBGShapefileServer::offset_type> CBGShapefileWrapper::build_offsets() const {
        return hpx::async<server::CBGShapefileServer::build_offsets_action>(get_id());
    }
}

typedef hpx::components::component<
        components::server::CBGShapefileServer
> cbg_shapefile_type;

HPX_REGISTER_COMPONENT(cbg_shapefile_type, CBGShapefileWrapper);
HPX_REGISTER_ACTION(::components::server::CBGShapefileServer::get_centroids_action);
HPX_REGISTER_ACTION(::components::server::CBGShapefileServer::build_offsets_action);