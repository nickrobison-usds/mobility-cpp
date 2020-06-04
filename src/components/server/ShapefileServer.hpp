//
// Created by Nicholas Robison on 6/3/20.
//

#ifndef MOBILITY_CPP_SHAPEFILESERVER_HPP
#define MOBILITY_CPP_SHAPEFILESERVER_HPP

#include <io/Shapefile.hpp>
#include <ogrsf_frmts.h>
#include <ogr_geometry.h>
#include "../include/components/serializers.hpp"
#include <hpx/include/components.hpp>

namespace components::server {
    class ShapefileServer: public hpx::components::locking_hook<hpx::components::component_base<ShapefileServer>> {

    public:
        typedef std::vector<std::pair<std::string, std::size_t>> offset_type;
        explicit ShapefileServer(std::string shapefile);

        std::vector<std::pair<std::string, OGRPoint>> get_centroids(const std::vector<std::string> &geoids);
        HPX_DEFINE_COMPONENT_ACTION(ShapefileServer, get_centroids);
        offset_type build_offsets() const;
        HPX_DEFINE_COMPONENT_ACTION(ShapefileServer, build_offsets);

    private:
        GDALDatasetUniquePtr _shapefile;
    };
}
HPX_REGISTER_ACTION_DECLARATION(::components::server::ShapefileServer::get_centroids_action, shapefile_server_get_centroids_action);
HPX_REGISTER_ACTION_DECLARATION(::components::server::ShapefileServer::build_offsets_action, shapefile_server_build_centroids_action);

#endif //MOBILITY_CPP_SHAPEFILESERVER_HPP
