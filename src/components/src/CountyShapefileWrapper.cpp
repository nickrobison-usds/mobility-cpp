//
// Created by Nicholas Robison on 9/14/20.
//

#include "components/CountyShapefileWrapper.hpp"

namespace components {
    CountyShapefileWrapper::CountyShapefileWrapper(const std::string& shapefile): client_base(hpx::new_<server::CountyShapefileServer>(hpx::find_here(), shapefile)) {
        // Not used
    };

    hpx::future<server::CountyShapefileServer::offset_type> CountyShapefileWrapper::build_offsets() const {
        return hpx::async<server::CountyShapefileServer::build_offsets_action>(get_id());
    }
}

typedef hpx::components::component<
        components::server::CountyShapefileServer
> county_shapefile_type;

HPX_REGISTER_COMPONENT(county_shapefile_type, CountyShapefileServer);
HPX_REGISTER_ACTION(::components::server::CountyShapefileServer::build_offsets_action);
