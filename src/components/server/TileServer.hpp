//
// Created by Nicholas Robison on 6/1/20.
//

#ifndef MOBILITY_CPP_TILESERVER_HPP
#define MOBILITY_CPP_TILESERVER_HPP

#include "../TileDimension.hpp"
#include "../include/components/data.hpp"
#include <hpx/include/components.hpp>
#include <blaze/math/DynamicMatrix.h>

#include <cstddef>

namespace components::server {

    class TileServer : public hpx::components::component_base<TileServer> {

    public:
        typedef blaze::DynamicMatrix<int> visit_matrix;
        typedef blaze::DynamicMatrix<double> distance_matrix;

        TileServer();

        void init(const std::string &filename, const TileDimension &dim, std::size_t num_nodes);

        HPX_DEFINE_COMPONENT_ACTION(TileServer, init);

    private:
        components::TileDimension _dim;
        std::vector<visit_matrix> _visits;
        std::vector<distance_matrix> _distances;

        static std::vector<weekly_pattern> extract_rows(const std::string &filename);
        std::tuple<std::uint64_t , std::uint64_t, double> computeDistance(const safegraph_location &row) const;
    };
}

HPX_REGISTER_ACTION_DECLARATION(::components::server::TileServer::init_action, tile_server_init_action);

#endif //MOBILITY_CPP_TILESERVER_HPP
