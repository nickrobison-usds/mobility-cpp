//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_COUNTYSHAPEFILESERVER_HPP
#define MOBILITY_CPP_COUNTYSHAPEFILESERVER_HPP

#include <hpx/include/components.hpp>
#include <ogrsf_frmts.h>

namespace components::server {
    class CountyShapefileServer : public hpx::components::component_base<CountyShapefileServer> {
    public:
        typedef std::vector<std::pair<std::string, std::size_t>> offset_type;

        explicit CountyShapefileServer(std::string shapefile);

        offset_type build_offsets() const;

        HPX_DEFINE_COMPONENT_ACTION(CountyShapefileServer, build_offsets);

    private:
        GDALDatasetUniquePtr _shapefile;
    };
}

HPX_REGISTER_ACTION_DECLARATION(::components::server::CountyShapefileServer::build_offsets_action, county_shapefile_server_build_offsets_action);


#endif //MOBILITY_CPP_COUNTYSHAPEFILESERVER_HPP
