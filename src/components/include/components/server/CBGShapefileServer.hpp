//
// Created by Nicholas Robison on 6/3/20.
//

#ifndef MOBILITY_CPP_CBGSHAPEFILESERVER_HPP
#define MOBILITY_CPP_CBGSHAPEFILESERVER_HPP

#include <shared/serializers.hpp>
#include <io/Shapefile.hpp>
#include <absl/container/flat_hash_map.h>
#include <ogrsf_frmts.h>
#include <ogr_geometry.h>
#include <hpx/include/components.hpp>

namespace components::server {
    class CBGShapefileServer: public hpx::components::component_base<CBGShapefileServer> {

    public:
        typedef std::vector<std::pair<std::string, std::size_t>> offset_type;
        typedef absl::flat_hash_map<std::string, OGRPoint> centroid_map;
        explicit CBGShapefileServer(std::string shapefile);

        std::vector<std::pair<std::string, OGRPoint>> get_centroids(const std::vector<std::string> &geoids);
        HPX_DEFINE_COMPONENT_ACTION(CBGShapefileServer, get_centroids);
        offset_type build_offsets() const;
        HPX_DEFINE_COMPONENT_ACTION(CBGShapefileServer, build_offsets);

    private:
        centroid_map build_centroid_map();
        GDALDatasetUniquePtr _shapefile;
        centroid_map _centroid_map;
    };
}
HPX_REGISTER_ACTION_DECLARATION(::components::server::CBGShapefileServer::get_centroids_action, cbg_shapefile_server_get_centroids_action);
HPX_REGISTER_ACTION_DECLARATION(::components::server::CBGShapefileServer::build_offsets_action, cbg_shapefile_server_build_offsets_action);

#endif //MOBILITY_CPP_CBGSHAPEFILESERVER_HPP
