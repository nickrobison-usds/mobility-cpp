//
// Created by Nicholas Robison on 6/1/20.
//

#ifndef MOBILITY_CPP_TILESERVER_HPP
#define MOBILITY_CPP_TILESERVER_HPP

#include "../TileConfiguration.hpp"
#include "shared/data.hpp"
#include <components/JoinedLocation.hpp>
#include <components/ShapefileWrapper.hpp>
#include <hpx/include/components.hpp>
#include <blaze/math/CompressedMatrix.h>

#include <cstddef>

class JoinedLocation;
class ShapefileWrapper;

namespace components::server {

    class TileServer : public hpx::components::component_base<TileServer> {

    public:
        typedef blaze::CompressedMatrix<int> visit_matrix;
        typedef blaze::CompressedMatrix<double> distance_matrix;

        TileServer(const std::string output_dir, const std::string output_name, const std::string cbg_shp, const std::string poi_parquet);

        void init(const TileConfiguration &dim, std::size_t num_nodes);

        HPX_DEFINE_COMPONENT_ACTION(TileServer, init);

    private:
        const std::string _output_dir;
        const std::string _output_name;
        JoinedLocation _l;
        ShapefileWrapper _s;
    };
}

HPX_REGISTER_ACTION_DECLARATION(::components::server::TileServer::init_action, tile_server_init_action);

#endif //MOBILITY_CPP_TILESERVER_HPP
